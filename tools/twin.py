#!/usr/bin/env python3
"""Port a matched twin's spelling into a draft, one differing hunk at a time.

  twin.py DRAFT.c FUNC TWIN.c TWINFUNC [--nonmatching] [--keep K]
  twin.py --seed FUNC TWIN.c TWINFUNC     (FUNC has no C yet: start it from the twin)

Families of functions (Ghost/Lun/Axe_TurnToward..., Skeleton/Mummy...) share
code over other struct fields. Once one member matches, its spelling usually
fixes the others, but the edits that got it there are often neutral one at a
time, so tools/fix.py's beam cannot rediscover them (Axe_UpdateTurnAnimation
stalled at 17 insns; the twin's three changes together match).

Both functions are printed the same way (pycparser), and their lines are
aligned with every identifier abstracted away. Equal lines teach a renaming
from the twin's names to the draft's (fields, locals, callees); each differing
hunk is then replaced by the twin's lines, renamed, alone and in combination,
and every candidate is compiled and scored against the ROM like fix.py does.
A match is written to build/twin/FUNC.c as the whole source file
(tools/writeback.py takes it); otherwise the best candidate goes there as
FUNC.partial.c, ready for tools/fix.py. Exit 0 on a match.
"""
import argparse
import collections
import copy
import difflib
import itertools
import os
import re
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix  # noqa: E402  (puts the permuter on sys.path)
import fixsearch  # noqa: E402
from romlib import ROOT  # noqa: E402
from perm_pycparser import c_ast as ca  # noqa: E402
from rewrites import set_child, walk  # noqa: E402
from src import ast_util  # noqa: E402

OUT = os.path.join(ROOT, "build", "twin")
KEEP = set("""auto break case char const continue default do double else enum extern float for goto if inline
int long register return short signed sizeof static struct switch typedef union unsigned void volatile while
u8 u16 u32 u64 s8 s16 s32 s64 bool8 bool16 bool32 vu8 vu16 vu32""".split())
TOKEN = re.compile(r"[A-Za-z_]\w*|0[xX][0-9A-Fa-f]+|\d+|->|<<|>>|[^\s\w]")


def tokens(line):
    return TOKEN.findall(line)


def is_ident(t):
    return re.match(r"[A-Za-z_]\w*$", t) is not None and t not in KEEP


def shape(line):
    return " ".join("$" if is_ident(t) else t for t in tokens(line))


def items_of(node):
    """The statement lists inside NODE: (owner, attribute) pairs whose value is a list of statements."""
    out = []
    if isinstance(node, ca.Compound):
        out.append((node, "block_items"))
    for _, child in node.children():
        if isinstance(child, ca.Node) and not isinstance(child, ca.Compound) and hasattr(child, "children"):
            if isinstance(child, (ca.If, ca.For, ca.While, ca.DoWhile, ca.Switch, ca.Label, ca.Case, ca.Default)):
                out += items_of(child)
        elif isinstance(child, ca.Compound):
            out += items_of(child)
    return out


def text_of(node):
    return ast_util.to_c(node).strip() if node is not None else ""


def align(tlist, dlist, sites, pairs):
    """Align two statement lists by shape; record replaceable draft runs (SITES) and same-shape
    statement pairs (PAIRS, to learn names from)."""
    tl, dl = tlist or [], dlist or []
    sm = difflib.SequenceMatcher(None, [shape(text_of(x)) for x in tl], [shape(text_of(x)) for x in dl],
                                 autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            pairs += [(tl[i1 + k], dl[j1 + k]) for k in range(i2 - i1)]
            continue
        sites.append((dl, j1, j2, tl[i1:i2]))
        # pair the run's statements by kind (an if with an if), then look inside each pair
        kinds = difflib.SequenceMatcher(None, [type(x).__name__ for x in tl[i1:i2]],
                                        [type(x).__name__ for x in dl[j1:j2]], autojunk=False)
        for ktag, a1, a2, b1, b2 in kinds.get_opcodes():
            if ktag != "equal":
                continue
            for k in range(a2 - a1):
                t, dd = tl[i1 + a1 + k], dl[j1 + b1 + k]
                if (i2 - i1, j2 - j1) != (1, 1):
                    sites.append((dl, j1 + b1 + k, j1 + b1 + k + 1, [t]))
                pairs.append((t, dd))
                inside(t, dd, sites, pairs)


def inside(t, dd, sites, pairs):
    """Align the statement lists inside two statements of the same kind."""
    if type(t) is not type(dd):
        return
    if isinstance(t, ca.If):
        if shape(text_of(t.cond)) == shape(text_of(dd.cond)):
            pairs.append((t.cond, dd.cond))
        for attr in ("iftrue", "iffalse"):
            a, b = getattr(t, attr), getattr(dd, attr)
            if isinstance(a, ca.Compound) and isinstance(b, ca.Compound):
                align(a.block_items, b.block_items, sites, pairs)
    elif isinstance(t, ca.Compound):
        align(t.block_items, dd.block_items, sites, pairs)
    elif isinstance(t, (ca.For, ca.While, ca.DoWhile)) and isinstance(t.stmt, ca.Compound) \
            and isinstance(dd.stmt, ca.Compound):
        align(t.stmt.block_items, dd.stmt.block_items, sites, pairs)


SCALAR = {"char": 1, "u8": 1, "s8": 1, "vu8": 1, "bool8": 1, "short": 2, "u16": 2, "s16": 2, "vu16": 2,
          "bool16": 2, "int": 4, "long": 4, "u32": 4, "s32": 4, "vu32": 4, "bool32": 4, "unsigned": 4,
          "signed": 4, "float": 4, "u64": 8, "s64": 8}


class Layout:
    """Struct layouts of one translation unit (GBA: natural alignment up to 4), enough to say which
    field sits at which offset. Bit-fields make a struct unknown."""

    def __init__(self, ast):
        self.typedefs, self.structs, self.cache = {}, {}, {}
        for n, _, _ in walk(ast):
            if isinstance(n, ca.Typedef):
                self.typedefs[n.name] = n.type
            if isinstance(n, (ca.Struct, ca.Union)) and n.name and n.decls is not None:
                self.structs.setdefault((type(n).__name__, n.name), n)

    def resolve(self, t):
        while isinstance(t, ca.TypeDecl) and isinstance(t.type, ca.IdentifierType) \
                and t.type.names[-1] in self.typedefs:
            t = self.typedefs[t.type.names[-1]]
        return t

    def struct_of(self, t):
        t = self.resolve(t)
        if isinstance(t, ca.TypeDecl) and isinstance(t.type, (ca.Struct, ca.Union)):
            s = t.type
            if s.decls is None and s.name:
                s = self.structs.get((type(s).__name__, s.name))
            return s
        return None

    def size_align(self, t):
        t = self.resolve(t)
        if isinstance(t, ca.PtrDecl):
            return 4, 4
        if isinstance(t, ca.ArrayDecl):
            sz, al = self.size_align(t.type)
            try:
                return sz * int(t.dim.value, 0), al
            except (AttributeError, ValueError, TypeError):
                return None
        if isinstance(t, ca.TypeDecl):
            if isinstance(t.type, ca.IdentifierType):
                names = t.type.names
                sz = 2 if "short" in names else 1 if "char" in names else SCALAR.get(names[-1])
                return (sz, min(sz, 4)) if sz else None
            if isinstance(t.type, ca.Enum):
                return 4, 4
            s = self.struct_of(t)
            fields = self.fields(s) if s is not None else None
            return (fields["$size"], fields["$align"]) if fields else None
        return None

    def fields(self, s):
        """{field: (offset, size, type)} plus $size and $align, or None."""
        if s is None or s.decls is None:
            return None
        if id(s) in self.cache:
            return self.cache[id(s)]
        self.cache[id(s)] = None  # recursion guard
        out, off, align = {}, 0, 1
        for dcl in s.decls:
            if dcl.bitsize is not None:
                return None
            sa = self.size_align(dcl.type)
            if sa is None:
                return None
            sz, al = sa
            align = max(align, al)
            if isinstance(s, ca.Union):
                out[dcl.name] = (0, sz, dcl.type)
                off = max(off, sz)
            else:
                off = (off + al - 1) // al * al
                if dcl.name:
                    out[dcl.name] = (off, sz, dcl.type)
                else:  # an anonymous struct or union: its fields are this one's
                    inner = self.fields(self.struct_of(dcl.type))
                    for k, v in (inner or {}).items():
                        if not k.startswith("$"):
                            out[k] = (off + v[0], v[1], v[2])
                off += sz
        out["$size"], out["$align"] = (off + align - 1) // align * align, align
        self.cache[id(s)] = out
        return out


def local_types(fn):
    """{name: declared type} for a function's parameters and locals."""
    out = {}
    for n, _, _ in walk(fn):
        if isinstance(n, ca.Decl) and n.name and not isinstance(n.type, ca.FuncDecl):
            out[n.name] = n.type
    params = fn.decl.type.args.params if fn.decl.type.args else []
    for prm in params:
        if isinstance(prm, ca.Decl) and prm.name:
            out[prm.name] = prm.type
    return out


def retarget_fields(node, tl, tlocals, dl, dlocals, names):
    """Rewrite the twin's `v->field` (v a pointer to a struct) for the draft's struct: the same name if
    the draft's struct has it, else the draft's field at the same offset and size, else a cast."""
    for n, parent, slot in list(walk(node)):
        if not (isinstance(n, ca.StructRef) and n.type == "->" and isinstance(n.name, ca.ID)):
            continue
        tt, dt = tlocals.get(n.name.name), dlocals.get(names.get(n.name.name, n.name.name))
        if not isinstance(tt, ca.PtrDecl) or not isinstance(dt, ca.PtrDecl):
            continue
        tf, df = tl.fields(tl.struct_of(tt.type)), dl.fields(dl.struct_of(dt.type))
        if not tf or n.field.name not in tf:
            continue
        if df and n.field.name in df and df[n.field.name][:2] == tf[n.field.name][:2]:
            continue
        off, sz, ftype = tf[n.field.name]
        same = [k for k, v in (df or {}).items() if not k.startswith("$") and v[0] == off and v[1] == sz]
        if same:
            n.field.name = same[0]
            names.pop(n.field.name, None)  # keep the draft's own field name through the renaming
            continue
        if not isinstance(tl.resolve(ftype), (ca.TypeDecl, ca.PtrDecl)) or tl.struct_of(ftype) is not None:
            continue
        base = ca.Cast(ca.Typename(None, [], None, [], ca.PtrDecl([], ca.TypeDecl(None, [], None,
                                                                                    ca.IdentifierType(["u8"])))),
                       ca.ID(n.name.name))
        addr = ca.BinaryOp("+", base, ca.Constant("int", hex(off)))
        ptr_t = copy.deepcopy(ftype)
        ptr = ca.Typename(None, [], None, [], ca.PtrDecl([], ptr_t))
        _set_declname(ptr_t)
        repl = ca.UnaryOp("*", ca.Cast(ptr, addr))
        set_child(parent, slot, repl)
    return node


def _set_declname(t):
    while t is not None:
        if isinstance(t, ca.TypeDecl):
            t.declname = None
            return
        t = getattr(t, "type", None)


def renamed(node, names):
    node = copy.deepcopy(node)
    for n, _, _ in walk(node):
        if isinstance(n, ca.ID) and n.name in names:
            n.name = names[n.name]
        elif isinstance(n, ca.StructRef) and isinstance(n.field, ca.ID) and n.field.name in names:
            n.field.name = names[n.field.name]
    return node


def rom_asm(addr):
    """The ROM disassembly of the function at ADDR (gen/code.s lines up to the next function)."""
    lines = open(os.path.join(ROOT, "gen", "code.s")).read().split("\n")
    at = f"@ 0x{addr:08X}"
    i = next(k for k, l in enumerate(lines) if l.endswith(at) and not l.startswith("_"))
    out = []
    for l in lines[i + 1:]:
        if re.match(r"\t\w*_func_start ", l):
            break
        out.append(l.split("@")[0].strip() if "@ =" not in l else l.strip())
    return [l for l in out if l and not l.startswith(".align")]


def asm_map(twin_asm, target_asm):
    """Name and constant correspondences between two ROM functions of the same shape:
    ({twin symbol: target symbol} from aligned `bl` and literal-pool symbols,
    [(twin value, target value)] from aligned immediates that differ)."""
    def norm(l):
        l = re.sub(r"\br\d+\b|\b(sb|sl|fp|ip|sp|lr)\b", "R", l)
        l = re.sub(r"#0x[0-9a-f]+|#\d+", "#K", l)
        l = re.sub(r"\b_[0-9A-F]{8}\b", "L", l)
        l = re.sub(r"=\w+", "=S", l)
        return re.sub(r"^bl \w+", "bl F", l)
    a, b = [norm(l) for l in twin_asm], [norm(l) for l in target_asm]
    names, consts = collections.Counter(), collections.Counter()
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, a, b, autojunk=False).get_opcodes():
        if tag not in ("equal", "replace") or i2 - i1 != j2 - j1:
            continue
        for x, y in zip(twin_asm[i1:i2], target_asm[j1:j2]):
            if norm(x) != norm(y):
                continue
            mx, my = re.match(r"bl (\w+)", x), re.match(r"bl (\w+)", y)
            if mx and my and mx.group(1) != my.group(1):
                names[(mx.group(1), my.group(1))] += 1
            sx, sy = re.search(r"=([A-Za-z_]\w*)", x), re.search(r"=([A-Za-z_]\w*)", y)
            if sx and sy and sx.group(1) != sy.group(1):
                names[(sx.group(1), sy.group(1))] += 1
            kx, ky = re.findall(r"#(0x[0-9a-f]+|\d+)", x), re.findall(r"#(0x[0-9a-f]+|\d+)", y)
            if x.split()[0] in ("movs", "cmp") and kx and ky and kx != ky:
                consts[(int(kx[-1], 0), int(ky[-1], 0))] += 1
    best = {}
    for (x, y), n in names.most_common():
        best.setdefault(x, y)
    return best, [k for k, _ in consts.most_common()]


def check_name(addr, name):
    """The function's symbol in build/boktai3.elf (the name it was built under)."""
    import subprocess
    out = subprocess.run(["arm-none-eabi-nm", os.path.join(ROOT, "build", "boktai3.elf")],
                         capture_output=True, text=True).stdout
    for l in out.splitlines():
        a, _, s = l.split(" ", 2)
        if int(a, 16) & ~1 == addr and not s.startswith(("_", "$")):
            return s
    return name


def seed(func, twin, twin_func, keep):
    """Start a draft of FUNC (no C yet) from its matched near-clone TWIN_FUNC: the twin's unit
    with the function renamed, callees and globals mapped from the two ROM listings, and each
    differing immediate tried in the C. Writes build/twin/FUNC.seed.c."""
    import isolate
    addrs = {r.split(",")[1]: int(r.split(",")[0], 16) for r in open(os.path.join(ROOT, "symbols", "functions.csv"))
             if re.match(r"[0-9A-F]{8},", r)}
    names_at = {v: k for k, v in addrs.items()}
    alias = dict(re.findall(r"^#define\s+(\w+)\s+sub_([0-9A-F]{8})\s*$", open(twin).read(), re.M))
    taddr = addrs.get(twin_func) or (int(alias[twin_func], 16) if twin_func in alias else int(twin_func[4:], 16))
    faddr = addrs.get(func) or int(func[4:], 16)
    names, consts = asm_map(rom_asm(taddr), rom_asm(faddr))
    text = fix.strip_draft(open(os.path.join(ROOT, isolate.ensure(twin, twin_func))).read())
    elf = check_name(faddr, names_at.get(faddr, func))
    text = re.sub(r"^#define\s+%s\s+sub_[0-9A-F]{8}\s*\n" % re.escape(twin_func), "", text, flags=re.M)
    text = re.sub(r"\b(%s|sub_%08X)\b" % (re.escape(twin_func), taddr), elf, text)
    for x, y in names.items():
        if re.search(r"\b%s\b" % re.escape(y), text) is None:
            proto = re.search(r"^[^\n;{}]*\b%s\s*\([^;{}]*\)\s*;" % re.escape(x), text, re.M)
            text = re.sub(r"\b%s\b" % re.escape(x), y, text)
            if proto is None:
                print(f"   {x} -> {y}: no prototype to copy; declare {y}")
        print(f"   map {x} -> {y}")
    os.makedirs(OUT, exist_ok=True)
    out = os.path.join(OUT, f"{func}.seed.c")
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
        d = fix.Draft(text, elf, wd)
        best = d.score()
        print(f"{func}: seeded from {twin_func}: {best[0]} differing insns ({best[2]} beyond registers)")
        span = fix.find_def(d.text, d.src_name)
        for x, y in consts:
            body = d.text[span[0]:span[1]]
            pat = re.compile(r"(?<![\w.])(%d|0x%X|0x%x)(?![\w.])" % (x, x, x))
            hits = list(pat.finditer(body))
            if len(hits) != 1:
                continue
            m = hits[0]
            cand = d.text[:span[0]] + body[:m.start()] + (hex(y) if y > 9 else str(y)) + body[m.end():] + d.text[span[1]:]
            cpath = os.path.join(wd, "cand", f"{func}.c")
            os.makedirs(os.path.dirname(cpath), exist_ok=True)
            open(cpath, "w").write(cand)
            try:
                _, _, cfn, _ = fixsearch.prepare(cpath, elf)
            except (Exception, SystemExit):
                continue
            sc = d.score(cfn)
            if sc and sc[0] < best[0]:
                print(f"   constant {x} -> {y}: {sc[0]} insns")
                best, text = sc, cand
                d = fix.Draft(text, elf, wd)
                span = fix.find_def(d.text, d.src_name)
        open(out, "w").write(text)
    print(f"seed: {os.path.relpath(out, ROOT)}; next: tools/step.py {os.path.relpath(out, ROOT)} --all, "
          f"then tools/fix.py on it")
    return 0 if best[0] == 0 else 1


def main():
    if sys.argv[1:2] == ["--seed"]:
        ap = argparse.ArgumentParser(description="seed a draft from a matched near-clone")
        ap.add_argument("--seed", action="store_true")
        ap.add_argument("func")
        ap.add_argument("twin")
        ap.add_argument("twin_func")
        ap.add_argument("--keep", type=int, default=5)
        a = ap.parse_args()
        return seed(a.func, a.twin, a.twin_func, a.keep)
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("draft")
    ap.add_argument("func")
    ap.add_argument("twin")
    ap.add_argument("twin_func")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--keep", type=int, default=5, help="best candidates to print")
    a = ap.parse_args()
    text = open(a.draft).read()
    if a.nonmatching:
        text = fix.strip_draft(text)
    twin_text = fix.strip_draft(open(a.twin).read())
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
        d = fix.Draft(text, a.func, wd)
        tdir = os.path.join(wd, "twin")
        os.makedirs(tdir)
        tpath = os.path.join(tdir, f"{a.twin_func}.c")
        open(tpath, "w").write(twin_text)
        _, tast, tfn, _ = fixsearch.prepare(tpath, a.twin_func)
        tlay, dlay = Layout(tast), Layout(d.ast)
        tlocals, dlocals = local_types(tfn), local_types(d.fn)
        sites, pairs = [], []
        align(tfn.body.block_items, d.fn.body.block_items, sites, pairs)
        votes = collections.defaultdict(collections.Counter)
        for t, dd in pairs:
            a_, b_ = tokens(text_of(t)), tokens(text_of(dd))
            if len(a_) == len(b_):
                for x, y in zip(a_, b_):
                    if is_ident(x) and is_ident(y):
                        votes[x][y] += 1
        names = {x: c.most_common(1)[0][0] for x, c in votes.items()}
        names[a.twin_func] = a.func
        base = d.score()
        print(f"{a.func}: {base[0]} differing insns; {len(sites)} statement runs differ from {a.twin_func} "
              f"({len(names)} names mapped)")

        def build(chosen):
            saved = []
            for lst, j1, j2, tnodes in sorted(chosen, key=lambda c: -c[1]):  # later runs first
                saved.append((lst, j1, list(lst)))
                lst[j1:j2] = [renamed(retarget_fields(copy.deepcopy(x), tlay, tlocals, dlay, dlocals, names), names)
                              for x in tnodes]
            fn_text = ast_util.to_c(d.fn).strip()
            for lst, _, old in reversed(saved):
                lst[:] = old
            return fn_text

        def disjoint(combo):
            spans = [(id(c[0]), c[1], c[2]) for c in combo]
            return all(not (x[0] == y[0] and x[1] < y[2] and y[1] < x[2]) for x, y in itertools.combinations(spans, 2))

        hunks = sites
        combos = [(h,) for h in hunks]
        if len(hunks) <= 12:
            combos += [c for c in itertools.combinations(hunks, 2) if disjoint(c)]
            combos += [c for c in itertools.combinations(hunks, 3) if disjoint(c)]
        results, seen, failed = [], set(), 0
        for combo in combos:
            fn_text = build(list(combo))
            if fn_text in seen:
                continue
            seen.add(fn_text)
            span = fix.find_def(d.text, d.src_name)
            body = fn_text
            if d.src_name != d.func:
                body = re.sub(r"\b" + d.func + r"\b", d.src_name, body, count=1)
            cand = d.text[:span[0]] + body + "\n" + d.text[span[1]:]
            cpath = os.path.join(wd, "cand", f"{a.func}.c")
            os.makedirs(os.path.dirname(cpath), exist_ok=True)
            open(cpath, "w").write(cand)
            try:
                _, _, cfn, _ = fixsearch.prepare(cpath, a.func)
            except (Exception, SystemExit):
                failed += 1
                continue
            sc = d.score(cfn)
            if sc is None:
                failed += 1
                continue
            results.append((sc, combo, cand))
            if sc[0] == 0:
                break
        results.sort(key=lambda r: (r[0][0], r[0][2], len(r[1])))
        if failed:
            print(f"   {failed} of {len(seen)} candidates do not compile (a twin field or helper the draft lacks)")
        for sc, combo, _ in results[:a.keep]:
            where = "; ".join(text_of(c[3][0]).split("\n")[0][:40] if c[3] else "(delete)" for c in combo)
            print(f"   {sc[0]:4} insns ({sc[2]} beyond registers)  twin's {where}")
        os.makedirs(OUT, exist_ok=True)
        if not results or results[0][0][0] >= base[0]:
            print("no twin hunk helps")
            return 1
        sc, combo, cand = results[0]
        out = os.path.join(OUT, a.func + (".c" if sc[0] == 0 else ".partial.c"))
        open(out, "w").write(cand)
        if sc[0] == 0:
            print(f"MATCH: {os.path.relpath(out, ROOT)} (tools/writeback.py {a.draft} {a.func} "
                  f"{os.path.relpath(out, ROOT)})")
            return 0
        print(f"best {sc[0]} insns: {os.path.relpath(out, ROOT)}; continue with tools/fix.py on it")
        return 1


if __name__ == "__main__":
    sys.exit(main())
