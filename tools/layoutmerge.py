#!/usr/bin/env python3
"""Merge every per-file declaration of a RAM object into one offset map.

  layoutmerge.py                      scan src/ and include/, write build/layouts/
  layoutmerge.py --summary [-n 40]    also print the ranking table
  layoutmerge.py gUnk_030042A0 ...    print the reports of these globals
  layoutmerge.py --range 03004280-0300523F
                                      only globals in this address range
  layoutmerge.py --no-rom             skip the ROM evidence (no build/boktai3.elf)

The decomp declares the same RAM object many times: each draft defines the
struct it needed, names the fields it understood and pads the rest
(`u8 unk20[0x38]`). This tool connects those declarations. For every RAM
global (and for the pointee of every global pointer) it collects each struct
used for it in any translation unit, places each member at the offset agbcc
gives it, and merges them per byte: which names, types and files touch each
byte, where files disagree (width or signedness at one offset), which bytes
one file leaves opaque but another names, and which bytes no file names.

Layouts come from the compiler, never from parsing C by hand: pycparser only
lists the member paths, and a probe compiled with the project's agbcc
(`(unsigned)&((T *)0)->a.b[0].c` and `sizeof`, in a const array) gives every
offset and size; bitfields are placed with a designated initializer that sets
the field to all ones. agbcc rounds struct sizes up to 4, so this matters.

Also harvested from the C: casts to a struct (`(struct X *)gUnk_02000710`),
typed accesses through byte pointers (`*(u16 *)(gUnk_02000710 + 0x30)`) and
constant indexes (`gUnk_030042A0[3]` of an `s16[]`).

ROM evidence (build/boktai3.elf, `arm-none-eabi-objdump -d`): a linear scan of
each function tracks registers loaded from literal pools. An access `[rX, #k]`
through a RAM literal L is recorded at L+k; a word loaded from a global and
then dereferenced gives accesses to that pointer's pointee. In agbcc's Thumb
code one literal plus several offsets is a struct, and one literal per field
is separate globals; the report counts both.

Output: build/layouts/<global>.md per global, build/layouts/SUMMARY.md (ranked
by opaque bytes that another file names) and build/layouts/layouts.json.
Per-file scans are cached in build/layouts/cache/ by preprocessed text, so a
rerun only preprocesses. See docs/LAYOUTS.md.
"""
import argparse
import bisect
import collections
import concurrent.futures
import copy
import gzip
import hashlib
import json
import os
import re
import shutil
import struct as pystruct
import subprocess
import sys
import tempfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)

try:
    import pycparser
    from pycparser import c_ast, c_generator
except ImportError:  # the project venv has it
    venv = os.path.join(ROOT, "build", "venv", "bin", "python")
    if os.path.exists(venv) and not os.environ.get("LAYOUTMERGE_REEXEC"):
        os.environ["LAYOUTMERGE_REEXEC"] = "1"
        os.execv(venv, [venv] + sys.argv)
    sys.exit("layoutmerge.py needs pycparser (build/venv)")

AGBCC = os.environ.get("AGBCC", os.path.join(ROOT, "build", "tools", "agbcc", "agbcc"))
CPP = ["cc", "-E"] if sys.platform == "darwin" else ["cpp"]
ELF = os.path.join(ROOT, "build", "boktai3.elf")
OUT = os.path.join(ROOT, "build", "layouts")
with open(os.path.abspath(__file__), "rb") as _f:
    VERSION = hashlib.sha1(_f.read()).hexdigest()  # a changed tool invalidates the per-file cache
CACHE = os.path.join(OUT, "cache", VERSION[:12])
CODE_END = 0x0824DAFC  # tools/build.py DATA_START

# A member whose own name says nobody knows what it is.
OPAQUE = re.compile(r"^_*(?:pad|padding|unk|unknown|unused|filler|fill|reserved|opaque|gap|dummy|field|"
                    r"Unk|Unknown|Pad|Field|Unused)(?:_?(?:0x)?[0-9A-Fa-f]*|[_A-Z0-9].*)$")
NUM = r"(0[xX][0-9a-fA-F]+|\d+)[uUlL]*"


def is_ram(v):
    return 0x02000000 <= v < 0x02040000 or 0x03000000 <= v < 0x03008000


# ---------------------------------------------------------------- C side

def preprocess(path):
    r = subprocess.run(CPP + ["-nostdinc", "-undef", "-DNONMATCHING", "-I", "include", "-iquote", ".", path],
                       cwd=ROOT, capture_output=True, text=True)
    return r.stdout if r.returncode == 0 else None


LINEMARK = re.compile(r'^#\s*(\d+)\s+"([^"]*)"')


def line_files(text):
    """(file, line) of every line of preprocessed text."""
    out, cur, ln = [], "?", 0
    for s in text.split("\n"):
        m = LINEMARK.match(s)
        if m:
            out.append((cur, ln))
            cur, ln = m.group(2), int(m.group(1)) - 1
            continue
        ln += 1
        out.append((cur, ln))
    return out


def _skip_balanced(t, i, open_ch, close_ch):
    """Index just past the bracket that closes t[i] (== open_ch)."""
    depth = 0
    n = len(t)
    while i < n:
        c = t[i]
        if c in "\"'":
            q = c
            i += 1
            while i < n and t[i] != q:
                i += 2 if t[i] == "\\" else 1
        elif c == open_ch:
            depth += 1
        elif c == close_ch:
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    return n


def _blank(seg):
    """Keep the line structure (and linemarkers) of text that is dropped."""
    return "\n".join(s if LINEMARK.match(s) else "" for s in seg.split("\n"))


GNU = re.compile(r"\b(__attribute__|__asm__|asm|__asm)\b")


def strip_gnu(t):
    """Drop __attribute__((...)) and asm(...) for pycparser."""
    out, i = [], 0
    for m in GNU.finditer(t):
        if m.start() < i:
            continue
        j = m.end()
        while j < len(t) and t[j] in " \t\n":
            j += 1
        while t.startswith(("volatile", "__volatile__"), j):
            j += 8 if t.startswith("volatile", j) else 12
            while j < len(t) and t[j] in " \t\n":
                j += 1
        if j < len(t) and t[j] == "(":
            k = _skip_balanced(t, j, "(", ")")
            out.append(t[i:m.start()])
            out.append(_blank(t[m.start():k]))
            i = k
    out.append(t[i:])
    t = "".join(out)
    t = re.sub(r"\b(__inline__|__inline|inline|__extension__|__volatile__|__const__|__signed__)\b",
               lambda m: {"__volatile__": "volatile", "__const__": "const", "__signed__": "signed"}.get(m.group(1), ""), t)
    return t


def strip_bodies(t):
    """Top-level declarations only: function bodies and initializers go (their
    lines stay blank), types and externs stay."""
    out, i, n = [], 0, len(t)
    last = ""  # last significant character emitted at depth 0
    while i < n:
        c = t[i]
        if c == "#" and (i == 0 or t[i - 1] == "\n"):
            j = t.find("\n", i)
            j = n if j < 0 else j
            out.append(t[i:j])
            i = j
            continue
        if c in "\"'":
            j = i + 1
            while j < n and t[j] != c:
                j += 2 if t[j] == "\\" else 1
            out.append(t[i:j + 1])
            i = j + 1
            last = c
            continue
        if c == "{":
            k = _skip_balanced(t, i, "{", "}")
            if last == ")":  # a function body
                out.append(";" + _blank(t[i:k]))
                last = ";"
            else:  # struct/union/enum body: keep
                out.append(t[i:k])
                last = "}"
            i = k
            continue
        if c == "=":  # an initializer: skip to the next , or ; at this level
            j, depth = i + 1, 0
            while j < n:
                d = t[j]
                if d in "([{":
                    depth += 1
                elif d in ")]}":
                    depth -= 1
                elif d in ",;" and depth == 0:
                    break
                elif d in "\"'":
                    j += 1
                    while j < n and t[j] != d:
                        j += 2 if t[j] == "\\" else 1
                j += 1
            out.append(_blank(t[i:j]))
            i = j
            continue
        if c == "(":
            k = _skip_balanced(t, i, "(", ")")
            out.append(t[i:k])
            last = ")"
            i = k
            continue
        out.append(c)
        if not c.isspace():
            last = c
        i += 1
    return "".join(out)


def pycparser_text(t):
    """pycparser reads `# 12 "file"` but not cpp's trailing flags."""
    return re.sub(r'^#\s*(\d+)\s+("[^"]*").*$', r"# \1 \2", t, flags=re.M)


GEN = c_generator.CGenerator()


class TU:
    """One translation unit's declarations of RAM globals and their layouts."""

    def __init__(self, ast):
        self.typedefs = {}  # name -> type node
        self.tags = {}  # ("struct"|"union", tag) -> Struct/Union node with decls
        self.anon = {}  # id(node) -> typedef name of an untagged aggregate
        for ext in ast.ext:
            self._collect(ext)

    def _collect(self, node):
        if isinstance(node, c_ast.Typedef):
            self.typedefs[node.name] = node.type
            base = node.type.type if isinstance(node.type, c_ast.TypeDecl) else None
            if isinstance(base, (c_ast.Struct, c_ast.Union)) and base.name is None:
                self.anon.setdefault(id(base), node.name)
        for _, child in node.children():
            self._collect(child)
        if isinstance(node, (c_ast.Struct, c_ast.Union)) and node.decls is not None and node.name:
            self.tags[(kind_of(node), node.name)] = node

    def resolve(self, t, depth=0):
        """Follow typedefs: TypeDecl(IdentifierType typedef) -> the typedef's type."""
        while depth < 32 and isinstance(t, c_ast.TypeDecl) and isinstance(t.type, c_ast.IdentifierType):
            names = t.type.names
            if len(names) == 1 and names[0] in self.typedefs:
                t = self.typedefs[names[0]]
                depth += 1
            else:
                break
        return t

    def aggregate(self, t):
        """(spelling or None, Struct node) if t is a struct/union type, else None."""
        spell = None
        if isinstance(t, c_ast.TypeDecl) and isinstance(t.type, c_ast.IdentifierType) and len(t.type.names) == 1 \
                and t.type.names[0] in self.typedefs:
            spell = t.type.names[0]
        r = self.resolve(t)
        if not isinstance(r, c_ast.TypeDecl) or not isinstance(r.type, (c_ast.Struct, c_ast.Union)):
            return None
        s = r.type
        if s.decls is None and s.name:
            s = self.tags.get((kind_of(s), s.name), s)
        if spell is None:  # None: an untagged struct, spelled by the caller
            spell = f"{kind_of(s)} {s.name}" if s.name else self.anon.get(id(s))
        return spell, s

    def scalar(self, t):
        """(width or None, signed or None, base) of a non-aggregate type."""
        r = self.resolve(t)
        if isinstance(r, c_ast.PtrDecl):
            return 4, False, "ptr"
        if isinstance(r, c_ast.TypeDecl):
            if isinstance(r.type, c_ast.Enum):
                return None, None, "enum"
            if isinstance(r.type, c_ast.IdentifierType):
                names = r.type.names
                w = 1 if "char" in names else 2 if "short" in names else 8 if names.count("long") == 2 else \
                    8 if "double" in names else 4
                if "unsigned" in names:
                    s = False
                elif "signed" in names or "char" not in names:
                    s = True
                else:
                    s = CHAR_SIGNED
                if "float" in names or "double" in names:
                    s = None
                if "void" in names:
                    w, s = None, None
                return w, s, "int"
        return None, None, "?"


CHAR_SIGNED = False  # set from the compiler in main()


def kind_of(s):
    return "union" if isinstance(s, c_ast.Union) else "struct"


def spell_type(t):
    t = copy.deepcopy(t)
    n = t
    while not isinstance(n, c_ast.TypeDecl) and hasattr(n, "type"):
        n = n.type
    if isinstance(n, c_ast.TypeDecl):
        n.declname = None
    try:
        return re.sub(r"\s+", " ", GEN.visit(c_ast.Typename(None, [], None, t))).strip()
    except Exception:
        return "?"


def flatten(tu, root, s, path, probes, leaves, containers, encl, seen):
    """Leaves of aggregate s, which `root` + path spells. path: list of ('m', name) / ('i', idx)."""
    if s.decls is None or id(s) in seen or len(path) > 24:
        return
    seen = seen | {id(s)}
    for d in s.decls:
        if not d.name:
            continue
        p = path + [("m", d.name)]
        t = d.type
        bits = None
        if d.bitsize is not None:
            try:
                bits = int(GEN.visit(d.bitsize), 0)
            except ValueError:
                bits = -1
        r = tu.resolve(t)
        agg = tu.aggregate(t)
        if agg and bits is None:
            flatten(tu, root, agg[1], p, probes, leaves, containers, encl, seen)
            if agg[1].decls is None:
                leaves.append(leaf(root, p, t, "incomplete", None, None, probes, encl, False))
            continue
        if isinstance(r, c_ast.ArrayDecl):
            et = r.type
            dims = 1
            while isinstance(tu.resolve(et), c_ast.ArrayDecl):  # u8 a[4][8]: one leaf, rows as elements
                et = tu.resolve(et).type
                dims += 1
            eagg = tu.aggregate(et)
            if eagg and dims == 1:
                cid = len(containers)
                containers.append({"size": len(probes), "elem": len(probes) + 1})
                probes.append(f"sizeof({root}{display(p)})")
                probes.append(f"sizeof({root}{display(p)}[0])")
                flatten(tu, root, eagg[1], p + [("i", 0)], probes, leaves, containers, encl + [cid], seen)
                continue
            w, sg, base = tu.scalar(et) if not eagg else (None, None, "agg")
            leaves.append(leaf(root, p, t, base, w, sg, probes, encl, True))
            continue
        w, sg, base = tu.scalar(t)
        leaves.append(leaf(root, p, t, base, w, sg, probes, encl, False, bits))


def leaf(root, p, t, base, w, sg, probes, encl, is_array, bits=None):
    lf = {"path": p, "ctype": spell_type(t), "base": base, "width": w, "signed": sg, "encl": encl,
          "array": is_array, "bits": bits}
    if bits is None:
        lf["off"] = len(probes)
        probes.append(f"(unsigned)&{root}{display(p)}")
        lf["size"] = len(probes)
        probes.append(f"sizeof({root}{display(p)})")
        if is_array:
            lf["esize"] = len(probes)
            probes.append(f"sizeof({root}{display(p)}[0])")
    else:
        lf["bf"] = p  # placed by a designated initializer
    return lf


def designator(p):
    s = "-1"
    for k, n in reversed(p):
        s = "{ " + (f".{n} = " if k == "m" else f"[{n}] = ") + s + " }"
    return s


def display(p):
    return "".join(f".{n}" if k == "m" else f"[{n}]" for k, n in p).lstrip(".")


ACC_STRUCT = r"\(\s*(?:const\s+|volatile\s+)*((?:struct|union)\s+\w+|\w+)\s*(?:const\s+|volatile\s+)*\*\s*\)\s*\(?\s*(&\s*)?({G})\b(?!\s*(?:\+|\[|->|\.))"
ACC_CAST = r"\*\s*\(\s*(?:const\s+|volatile\s+)*(\w+)\s*(?:const\s+|volatile\s+)*\*\s*\)\s*\(\s*(?:\(\s*(?:const\s+)?(?:u8|s8|char|void|vu8)\s*\*\s*\)\s*)?(&\s*)?\(?\s*({G})\s*\)?\s*\+\s*" + NUM + r"\s*\)"
ACC_INDEX = r"(?<![&\w])({G})\s*\[\s*" + NUM + r"\s*\](?!\s*(?:\.|\[))"


def scan_tu(path, globals_):
    """Declarations, struct layouts and C accesses of RAM globals in one file."""
    text = preprocess(path)
    if text is None:
        return {"error": "preprocess failed"}
    names = [g for g in globals_ if g in text]
    if not names:
        return {}
    key = hashlib.sha1((text + " ".join(names)).encode()).hexdigest()
    cache = os.path.join(CACHE, key[:2], key + ".json.gz")
    if os.path.exists(cache):
        with gzip.open(cache, "rt") as f:
            return json.load(f)
    res = _scan_tu(path, text, set(names))
    os.makedirs(os.path.dirname(cache), exist_ok=True)
    tmp = cache + f".{os.getpid()}"
    with gzip.open(tmp, "wt", compresslevel=3) as f:
        json.dump(res, f)
    os.replace(tmp, cache)
    return res


def prune_cache():
    """Scans made by other versions of this tool are never read again."""
    top = os.path.dirname(CACHE)
    if os.path.isdir(top):
        for d in os.listdir(top):
            if os.path.join(top, d) != CACHE:
                shutil.rmtree(os.path.join(top, d), ignore_errors=True)


def _scan_tu(path, text, names):
    lines = line_files(text)
    body = strip_bodies(text)
    try:
        ast = pycparser.CParser().parse(pycparser_text(strip_gnu(body)), path)
    except Exception as e:  # pycparser.plyparser.ParseError and friends
        return {"error": f"parse: {str(e)[:200]}"}
    tu = TU(ast)

    def where(coord):
        return (coord.file, coord.line) if coord else ("?", 0)

    decls = []  # (global, type node, "decl"|"cast", file, line)
    for ext in ast.ext:
        if isinstance(ext, c_ast.Decl) and ext.name in names and not isinstance(ext.type, c_ast.FuncDecl):
            f, ln = where(ext.coord)
            decls.append((ext.name, ext.type, "decl", f, ln))
    # casts and constant indexes in function bodies (the full text, before stripping)
    gpat = "|".join(sorted(map(re.escape, names)))
    tl = text.split("\n")
    ctx_lines = []
    for i, s in enumerate(tl):
        if any(g in s for g in names):
            ctx_lines.append(i)
    accesses = []
    gkind = {}  # global -> resolved declared type in this TU
    for g, t, *_ in decls:
        gkind.setdefault(g, t)
    for i in ctx_lines:
        s = tl[i]
        f, ln = lines[i]
        for m in re.finditer(ACC_STRUCT.format(G=gpat), s):
            ty, amp, g = m.groups()
            td = c_ast.TypeDecl(None, [], None, c_ast.Struct(ty.split()[1], None) if ty.startswith("struct ") else
                                c_ast.Union(ty.split()[1], None) if ty.startswith("union ") else
                                c_ast.IdentifierType([ty]))
            if tu.aggregate(td) is None:
                continue
            direct = amp or isinstance(tu.resolve(gkind.get(g)), c_ast.ArrayDecl)  # an array is its address
            decls.append((g, td if direct else c_ast.PtrDecl([], td), "cast", f, ln))
        for m in re.finditer(ACC_CAST.format(G=gpat), s):
            ty, amp, g, k = m.groups()
            w, sg, base = tu.scalar(c_ast.TypeDecl(None, [], None, c_ast.IdentifierType([ty])))
            if w is None:
                continue
            gt = tu.resolve(gkind.get(g)) if g in gkind else None
            if amp:
                space = "direct"
            elif isinstance(gt, c_ast.ArrayDecl):
                space = "direct"
            elif isinstance(gt, c_ast.PtrDecl):
                space = "pointee"
                pw = tu.scalar(gt.type)
                if tu.aggregate(gt.type) or (pw[0] not in (1, None) and pw[2] != "?"):
                    continue  # pointer arithmetic scaled by an element size
            else:
                continue
            accesses.append({"global": g, "space": space, "off": int(k, 0), "width": w, "signed": sg,
                             "ctype": ty, "form": "cast", "file": f, "line": ln})
        for m in re.finditer(ACC_INDEX.format(G=gpat), s):
            g, k = m.groups()
            gt = tu.resolve(gkind.get(g)) if g in gkind else None
            if isinstance(gt, c_ast.ArrayDecl):
                space = "direct"
            elif isinstance(gt, c_ast.PtrDecl):
                space = "pointee"
            else:
                continue
            et = gt.type
            if tu.aggregate(et) or isinstance(tu.resolve(et), (c_ast.ArrayDecl,)):
                continue
            w, sg, base = tu.scalar(et)
            if not w:
                continue
            accesses.append({"global": g, "space": space, "off": int(k, 0) * w, "width": w, "signed": sg,
                             "ctype": spell_type(et), "form": "index", "file": f, "line": ln})

    # layouts of the aggregates used for these globals
    probes, structs, out_decls = [], {}, []
    bitfields = []  # (struct key, leaf index, spelling, path)
    for g, t, how, f, ln in decls:
        r = tu.resolve(t)
        space, count, obj = "direct", None, g  # obj: an expression of the type described
        if isinstance(r, c_ast.PtrDecl):
            space, inner, obj = "pointee", r.type, f"*{g}"
        elif isinstance(r, c_ast.ArrayDecl):
            inner, obj = r.type, f"{g}[0]"
            ri = tu.resolve(inner)
            if isinstance(ri, c_ast.PtrDecl):  # struct X *g[N]: the pointee of each element
                space, inner, obj = "pointee", ri.type, f"*{g}[0]"
            else:
                count = "array"
        else:
            inner = t
        if how == "cast":
            obj = None
        agg = tu.aggregate(inner)
        d = {"global": g, "space": space, "how": how, "file": f, "line": ln, "ctype": spell_type(t)}
        if agg:
            spell, s = agg
            sf, sl = where(s.coord)
            if spell is None and obj is None:
                continue
            skey = f"{sf}:{sl}:{spell or kind_of(s) + ' <untagged>'}"
            spell = spell or f"__typeof__({obj})"
            d["struct"] = skey
            if count == "array":  # struct X g[N]: N copies, when the declaration says N
                d["count"] = len(probes)
                probes.append(f"sizeof({g}) / sizeof({g}[0])")
            if skey not in structs and s.decls is not None:
                leaves, containers = [], []
                st = {"spell": spell, "file": sf, "line": sl, "size": len(probes), "leaves": leaves,
                      "containers": containers}
                probes.append(f"sizeof({spell})")
                flatten(tu, f"(({spell} *)0)->", s, [], probes, leaves, containers, [], frozenset())
                for li, lf in enumerate(leaves):
                    if "bf" in lf:
                        bitfields.append((skey, li, spell, lf["bf"]))
                structs[skey] = st
            elif skey not in structs:
                d["incomplete"] = True
        else:
            while isinstance(tu.resolve(inner), c_ast.ArrayDecl):  # u16 g[16][16]: elements are u16
                inner = tu.resolve(inner).type
            w, sg, base = tu.scalar(inner)
            d.update(width=w, signed=sg, base=base, array=count == "array")
            if space == "direct" and how == "decl":
                d["sizeof"] = len(probes)
                probes.append(f"sizeof({g})")
        out_decls.append(d)

    if probes or bitfields:
        vals, bfbytes = run_probe(body, probes, [(spell, p) for _, _, spell, p in bitfields])
    else:
        vals, bfbytes = [], []
    for d in out_decls:
        for k in ("count", "sizeof"):
            if k in d:
                d[k] = vals[d[k]]
    for st in structs.values():
        st["size"] = vals[st["size"]]
        for c in st["containers"]:
            c["size"], c["elem"] = vals[c["size"]], vals[c["elem"]]
        for lf in st["leaves"]:
            for k in ("off", "size", "esize"):
                if k in lf:
                    lf[k] = vals[lf[k]]
            lf["name"] = display(lf.pop("path"))
            lf.pop("bf", None)
    for (skey, li, _, _), b in zip(bitfields, bfbytes):
        lf = structs[skey]["leaves"][li]
        if b is None:
            lf["off"] = None
            continue
        bits = [8 * i + j for i, v in enumerate(b) for j in range(8) if v >> j & 1]
        if not bits:
            lf["off"] = None
            continue
        lo, hi = bits[0], bits[-1] + 1
        lf["off"], lf["size"] = lo // 8, (hi + 7) // 8 - lo // 8
        lf["bitpos"], lf["bits"] = lo, hi - lo
    return {"decls": out_decls, "structs": structs, "accesses": accesses}


PROBE_WORD = re.compile(r"^\s*\.(word|4byte|long|short|2byte|hword|byte|space)\s+(.*)$")


def run_probe(body, probes, bitfields):
    """Compile body + probes with agbcc; values (None where agbcc refused)."""
    src = [body, '\n# 1 "__probe__"\n', "const unsigned __lm_probe[] = {\n"]
    src += [p + ",\n" for p in probes]
    src.append("0 };\n")
    for i, (spell, p) in enumerate(bitfields):
        src.append(f"const {spell} __lm_bf{i} = {designator(p)};\n")
    with tempfile.TemporaryDirectory() as td:
        ipath, spath = os.path.join(td, "p.i"), os.path.join(td, "p.s")
        with open(ipath, "w") as f:
            f.write("".join(src))
        r = subprocess.run([AGBCC, "-O2", "-mthumb-interwork", ipath, "-o", spath], capture_output=True, text=True,
                           stdin=subprocess.DEVNULL)
        if not os.path.exists(spath):
            return [None] * len(probes), [None] * len(bitfields)
        asm = open(spath).read()
    bad = set()
    for m in re.finditer(r"^__probe__:(\d+):", r.stderr, re.M):
        ln = int(m.group(1))
        if ln >= 3:
            n = ln - 3
            if n < len(probes):
                bad.add(n)
            else:
                bad.add(("bf", n - len(probes) - 1))
    blobs = asm_objects(asm)
    words = blobs.get("__lm_probe", b"")
    vals = []
    for i in range(len(probes)):
        if i in bad or 4 * i + 4 > len(words):
            vals.append(None)
        else:
            vals.append(pystruct.unpack_from("<I", words, 4 * i)[0])
    bfs = []
    for i in range(len(bitfields)):
        b = blobs.get(f"__lm_bf{i}")
        bfs.append(None if b is None or ("bf", i) in bad else list(b))
    return vals, bfs


def asm_objects(asm):
    """Bytes of each data object in agbcc's assembly output."""
    out, cur = {}, None
    for s in asm.split("\n"):
        m = re.match(r"^(\w+):\s*$", s)
        if m:
            cur = m.group(1)
            out[cur] = bytearray()
            continue
        m = PROBE_WORD.match(s)
        if m and cur is not None:
            op, arg = m.groups()
            if op == "space":
                out[cur] += bytes(int(arg.split(",")[0], 0))
                continue
            w = {"word": 4, "4byte": 4, "long": 4, "short": 2, "2byte": 2, "hword": 2, "byte": 1}[op]
            for a in arg.split(","):
                try:
                    v = int(a.strip(), 0)
                except ValueError:
                    v = 0
                out[cur] += (v & ((1 << 8 * w) - 1)).to_bytes(w, "little")
        elif s.strip().startswith((".globl", ".text", ".section", ".data")):
            cur = None
    return out


def char_signed():
    with tempfile.TemporaryDirectory() as td:
        ipath, spath = os.path.join(td, "c.i"), os.path.join(td, "c.s")
        open(ipath, "w").write("const int __lm_c = ((char)-1 < 0);\n")
        subprocess.run([AGBCC, "-O2", ipath, "-o", spath], capture_output=True)
        return asm_objects(open(spath).read()).get("__lm_c", b"\0")[0] == 1


# ---------------------------------------------------------------- ROM side

INSN = re.compile(r"^\s*([0-9a-f]+):\s+(?:[0-9a-f]{4}\s?){1,2}\s+(\S+)\s*(.*)$")
FUNC = re.compile(r"^([0-9a-f]{8}) <([^>]+)>:")
REG = {f"r{i}": i for i in range(13)}
REG.update(sb=9, sl=10, fp=11, ip=12)
WIDTH = {"ldr": (4, None), "str": (4, None), "ldrh": (2, False), "strh": (2, None), "ldrb": (1, False),
         "strb": (1, None), "ldrsh": (2, True), "ldrsb": (1, True)}


def rom_accesses(elf_path):
    """[(literal, offset, width, signed, load?, func, pointer?)] from the Thumb code."""
    data = open(elf_path, "rb").read()
    # ELF32: find .text by section header
    shoff, = pystruct.unpack_from("<I", data, 0x20)
    shnum, shstrndx = pystruct.unpack_from("<HH", data, 0x30)
    sects = [pystruct.unpack_from("<IIIIIIIIII", data, shoff + 40 * i) for i in range(shnum)]
    text = next(s for s in sects if s[3] == 0x08000000)
    base, off, size = text[3], text[4], text[5]

    def word(a):
        if base <= a < base + size - 3:
            return pystruct.unpack_from("<I", data, off + a - base)[0]
        return None

    dis = subprocess.run(["arm-none-eabi-objdump", "-d", f"--stop-address={CODE_END:#x}", elf_path],
                         capture_output=True, text=True).stdout
    out = []
    func, body = None, []
    for s in dis.split("\n"):
        m = FUNC.match(s)
        if m:
            if not m.group(2).startswith("_0"):
                if func:
                    _scan_func(func, body, word, out)
                func, body = m.group(2), []
            body.append(("label", int(m.group(1), 16)))
            continue
        m = INSN.match(s)
        if m:
            body.append((int(m.group(1), 16), m.group(2), m.group(3)))
    if func:
        _scan_func(func, body, word, out)
    return out


def _scan_func(func, body, word, out):
    pools = set()
    for it in body:
        if it[0] != "label" and it[1] == "ldr" and "[pc" in it[2]:
            m = re.search(r"@ \(([0-9a-f]+)", it[2])
            if m:
                a = int(m.group(1), 16)
                pools.update(range(a, a + 4))
    st = {}  # reg -> ("addr", A) | ("ptr", A, off) | ("const", v)
    flow = True
    for it in body:
        if it[0] == "label":
            if not flow:
                st = {}
            continue
        a, op, args = it
        if a in pools:
            st, flow = {}, False
            continue
        flow = True
        args = args.split("@")[0].strip()
        ops = [x.strip() for x in re.split(r",(?![^\[{]*[\]}])", args)] if args else []
        dest = REG.get(ops[0]) if ops else None
        try:
            if op == "ldr" and "[pc" in args:
                m = re.search(r"@ \(([0-9a-f]+)", it[2])
                v = word(int(m.group(1), 16)) if m else None
                st[dest] = ("addr", v) if v is not None and is_ram(v) else ("const", v) if v is not None else None
                continue
            if op in WIDTH and len(ops) == 2 and ops[1].startswith("["):
                inner = [x.strip() for x in ops[1].strip("[]").split(",")]
                b = st.get(REG.get(inner[0]))
                k = 0
                if len(inner) > 1:
                    if inner[1].startswith("#"):
                        k = int(inner[1][1:], 0)
                    else:
                        o = st.get(REG.get(inner[1]))
                        if o and o[0] == "const" and b and b[0] != "const":
                            pass
                        elif o and o[0] != "const" and b and b[0] == "const":
                            b, o = o, b
                        else:
                            b = None
                        if b:
                            k = o[1]
                w, sg = WIDTH[op]
                load = op.startswith("ldr")
                if b and b[0] == "addr":
                    out.append((b[1], k, w, sg, load, func, False))
                elif b and b[0] == "ptr":
                    out.append((b[1], b[2] + k, w, sg, load, func, True))
                if load:
                    st[dest] = ("ptr", b[1] + k, 0) if b and b[0] == "addr" and w == 4 else None
                continue
            if op in ("ldmia", "stmia"):
                rn = REG.get(ops[0].rstrip("!"))
                b = st.get(rn)
                regs = re.findall(r"r\d+", args.split("{", 1)[1])
                if b and b[0] in ("addr", "ptr"):
                    for i in range(len(regs)):
                        if b[0] == "addr":
                            out.append((b[1], 4 * i, 4, None, op == "ldmia", func, False))
                        else:
                            out.append((b[1], b[2] + 4 * i, 4, None, op == "ldmia", func, True))
                if op == "ldmia":
                    for r in regs:
                        st[REG[r]] = None
                if ops[0].endswith("!"):
                    st[rn] = shift(b, 4 * len(regs))
                continue
            if op in ("movs", "mov") and len(ops) == 2:
                if ops[1].startswith("#"):
                    st[dest] = ("const", int(ops[1][1:], 0))
                else:
                    st[dest] = st.get(REG.get(ops[1]))
                continue
            if op in ("adds", "add", "subs", "sub") and dest is not None and dest < 13:
                sign = -1 if op.startswith("sub") else 1
                if len(ops) == 2:  # adds rd, #imm / add rd, rm
                    src, other = st.get(dest), ops[1]
                else:
                    src, other = st.get(REG.get(ops[1])), ops[2]
                if other.startswith("#"):
                    st[dest] = shift(src, sign * int(other[1:], 0))
                else:
                    o = st.get(REG.get(other))
                    if src and o and sign == 1 and (src[0] == "const") != (o[0] == "const"):
                        p, c = (o, src) if src[0] == "const" else (src, o)
                        st[dest] = shift(p, c[1])
                    elif src and o and src[0] == o[0] == "const":
                        st[dest] = ("const", src[1] + sign * o[1])
                    else:
                        st[dest] = None
                continue
            if op == "lsls" and len(ops) == 3 and ops[2].startswith("#"):
                s0 = st.get(REG.get(ops[1]))
                st[dest] = ("const", (s0[1] << int(ops[2][1:], 0)) & 0xFFFFFFFF) if s0 and s0[0] == "const" else None
                continue
            if op in ("bl", "blx"):
                for r in (0, 1, 2, 3, 12):
                    st[r] = None
                continue
            if op in ("b", "b.n", "bx") or (op == "pop" and "pc" in args):
                st, flow = {}, False
                continue
            if op == "pop":
                for r in re.findall(r"r\d+", args):
                    st[REG[r]] = None
                continue
            if op.startswith(("b", "cmp", "cmn", "tst", "str", "push", "svc", "swi")):
                continue
            if dest is not None:
                st[dest] = None
        except (ValueError, TypeError, KeyError, IndexError):
            if dest is not None:
                st[dest] = None


def shift(v, k):
    if not v or v[1] is None:
        return None
    if v[0] == "const":
        return ("const", (v[1] + k) & 0xFFFFFFFF)
    if v[0] == "addr":
        return ("addr", v[1] + k)
    return ("ptr", v[1], v[2] + k)


# ---------------------------------------------------------------- merge

def ram_symbols():
    out = {}
    r = subprocess.run(["arm-none-eabi-nm", ELF], capture_output=True, text=True)
    for s in r.stdout.split("\n"):
        p = s.split()
        if len(p) == 3:
            v = int(p[0], 16)
            if is_ram(v) and not p[2].startswith(("_0", "$")):
                out[p[2]] = v
    return out


def sources():
    out = []
    for d in ("src", "include"):
        for dp, _, fs in os.walk(os.path.join(ROOT, d)):
            for f in sorted(fs):
                if f.endswith(".c"):
                    out.append(os.path.relpath(os.path.join(dp, f), ROOT))
    return sorted(out)


def headers():
    """Headers no C file includes would go unseen: scan each through a stub."""
    return sorted(os.path.join("include", f) for f in os.listdir(os.path.join(ROOT, "include")) if f.endswith(".h"))


def scan_all(globals_, jobs):
    files = sources()
    stubs = []
    stubdir = os.path.join(OUT, "stubs")
    os.makedirs(stubdir, exist_ok=True)
    for h in headers():
        p = os.path.join(stubdir, os.path.basename(h)[:-2] + ".c")
        txt = f'#include "global.h"\n#include "{os.path.basename(h)}"\n'
        if not os.path.exists(p) or open(p).read() != txt:
            open(p, "w").write(txt)
        stubs.append(os.path.relpath(p, ROOT))
    results = {}
    with concurrent.futures.ProcessPoolExecutor(jobs, initializer=_init, initargs=(CHAR_SIGNED,)) as ex:
        futs = {ex.submit(scan_tu, f, globals_): f for f in files + stubs}
        for fu in concurrent.futures.as_completed(futs):
            results[futs[fu]] = fu.result()
    return results


def _init(cs):
    global CHAR_SIGNED
    CHAR_SIGNED = cs


PLACEHOLDER = re.compile(r"^[A-Za-z]_?(?:0x)?(?=[0-9A-Fa-f]*[0-9])[0-9A-Fa-f]{2,4}$")  # f54, a720, b_1C


def opaque(name, lf=None):
    """A member nobody has named: unk/pad/field..., a letter plus an offset
    (f54), or a byte array named by one letter (u8 f[0x868])."""
    own = re.split(r"[\[]", name.rsplit(".", 1)[-1])[0]
    if OPAQUE.match(own) or PLACEHOLDER.match(own):
        return True
    return bool(lf and lf["array"] and lf.get("width") == 1 and len(own) == 1)


def placements(st):
    """Expand a struct's leaves over its arrays of structs: (off, size, leaf, name)."""
    out = []
    for lf in st["leaves"]:
        if lf.get("off") is None:
            continue
        size = lf.get("size") or lf.get("width") or 1
        reps = [(0, lf["name"])]
        for k, cid in enumerate(lf["encl"]):
            c = st["containers"][cid]
            if not c.get("elem") or c.get("size") is None:
                continue
            n = c["size"] // c["elem"]
            if n * len(reps) > 4096:
                break
            reps = [(base + i * c["elem"], nth_index(name, k, i)) for base, name in reps for i in range(n)]
        out += [(lf["off"] + base, size, lf, name) for base, name in reps]
    return out


def nth_index(name, k, i):
    """Replace the k-th "[0]" of name (the k-th enclosing array) with [i]."""
    parts = name.split("[0]")
    if k + 1 >= len(parts):
        return name
    return "".join(p + (f"[{i}]" if j == k else "[0]") for j, p in enumerate(parts[:-1])) + parts[-1]


def merge(results, alias):
    """Per (global, space): decls, struct fields placed at offsets, C accesses.
    alias maps every name to the canonical name at its address."""
    G = collections.defaultdict(lambda: {"decls": [], "fields": [], "access": [], "sizes": {}})
    structs, errors, seen = {}, {}, set()
    for f, r in sorted(results.items()):
        if "error" in r:
            errors[f] = r["error"]
            continue
        structs.update(r.get("structs", {}))
        for d in r.get("decls", []):
            d["global"] = alias.get(d["global"], d["global"])
            k = (d["global"], d["space"], d["file"], d["line"], d["how"], d.get("struct"), d["ctype"])
            if k not in seen:
                seen.add(k)
                G[(d["global"], d["space"])]["decls"].append(d)
        for a in r.get("accesses", []):
            a["global"] = alias.get(a["global"], a["global"])
            k = (a["global"], a["space"], a["file"], a["line"], a["off"], a["width"])
            if k not in seen:
                seen.add(k)
                G[(a["global"], a["space"])]["access"].append(a)
    placed = {key: placements(st) for key, st in structs.items()}
    for (g, space), e in G.items():
        users = collections.defaultdict(set)  # (struct key, element count) -> files using it here
        for d in e["decls"]:
            if d.get("struct") in structs:
                users[(d["struct"], d.get("count") or 1)].add(d["file"])
        for (skey, count), files in users.items():
            st = structs[skey]
            stride = st["size"] or 0
            if not stride or count * stride > 0x10000:
                count = 1
            for i in range(count):
                for off, size, lf, name in placed[skey]:
                    e["fields"].append({
                        "off": i * stride + off, "size": size, "name": f"[{i}].{name}" if count > 1 else name,
                        "ctype": lf["ctype"], "signed": lf["signed"], "base": lf["base"], "array": lf["array"],
                        "width": lf.get("esize") if lf["array"] else
                        size if lf.get("bits") is not None else lf.get("width") or size,
                        "bits": lf.get("bits"), "bitpos": lf.get("bitpos"), "struct": skey, "spell": st["spell"],
                        "files": sorted(files), "opaque": opaque(name, lf)})
            e["sizes"][skey] = (st["size"] or 0) * count if count > 1 else st["size"]
        if space == "direct":  # a scalar or an array of scalars names its bytes; u8 g[] leaves them opaque
            scal = collections.defaultdict(set)
            for d in e["decls"]:
                if "struct" not in d and d.get("width"):
                    size = d.get("sizeof") or (None if d["array"] else d["width"])  # None: g[], to the extent
                    scal[(d["ctype"], size, d["width"], d["signed"], d["base"], d["array"])].add(d["file"])
            for (ct, size, w, sg, base, arr), files in scal.items():
                e["fields"].append({"off": 0, "size": size, "name": g, "ctype": ct, "signed": sg, "base": base,
                                    "array": arr, "width": w, "bits": None, "bitpos": None, "struct": None,
                                    "spell": ct, "files": sorted(files), "opaque": arr and w == 1})
    return G, structs, errors


def extent(e):
    ext = 0
    for skey, s in e.get("sizes", {}).items():
        if s:
            ext = max(ext, s)
    for f in e["fields"]:
        if f["size"] is not None and not f.get("to_extent"):
            ext = max(ext, f["off"] + f["size"])
    for a in e["access"]:
        ext = max(ext, a["off"] + a["width"])
    return ext


def analyze(g, space, e, addr, rom):
    """Byte map, conflicts, opaque ranges for one global/space."""
    ext = extent(e)
    rom_here = rom or []
    for a in rom_here:
        ext = max(ext, a[1] + a[2])
    ext = min(ext, 0x10000)
    for f in e["fields"]:  # an unsized array (u16 g[]) reaches as far as anything else does
        if f["size"] is None or f.get("to_extent"):
            f["size"], f["to_extent"] = max(ext - f["off"], f["width"] or 1), True
    named = [set() for _ in range(ext)]  # files that name this byte
    opq = [set() for _ in range(ext)]
    for f in e["fields"]:
        tgt = opq if f["opaque"] else named
        for b in range(f["off"], min(f["off"] + f["size"], ext)):
            tgt[b].update(f["files"])
    romb = [0] * ext
    for a in rom_here:
        for b in range(a[1], min(a[1] + a[2], ext)):
            romb[b] += 1
    cacc = [0] * ext
    for a in e["access"]:
        for b in range(a["off"], min(a["off"] + a["width"], ext)):
            cacc[b] += 1
    # opaque bytes named elsewhere: some file leaves it opaque, a different file names it
    elsewhere = [b for b in range(ext) if opq[b] and named[b] - opq[b]]
    covered = [bool(named[b] or opq[b]) for b in range(ext)]
    unnamed = [b for b in range(ext) if not named[b]]
    # conflicts among named scalar fields starting at one offset
    by_off = collections.defaultdict(list)
    for f in e["fields"]:
        if not f["opaque"] and f["base"] in ("int", "ptr", "enum") and f.get("bits") is None:
            by_off[f["off"]].append(f)
    conflicts = []
    for off, fs in sorted(by_off.items()):
        widths = {f["width"] for f in fs if f["width"]}
        signs = {f["signed"] for f in fs if f["signed"] is not None and f["base"] == "int"}
        if len(widths) > 1 or len(signs) > 1:
            conflicts.append((off, fs, widths, signs))
    return {"extent": ext, "named": named, "opq": opq, "romb": romb, "cacc": cacc, "elsewhere": elsewhere,
            "unnamed": unnamed, "conflicts": conflicts, "covered": covered}


def ranges(bytes_, key=lambda b: None):
    """[lo, hi) runs of consecutive bytes with the same key."""
    out = []
    for b in bytes_:
        if out and out[-1][1] == b and key(b) == out[-1][2]:
            out[-1][1] = b + 1
        else:
            out.append([b, b + 1, key(b)])
    return [(lo, hi) for lo, hi, _ in out]


def rom_cell(counter):
    """ROM accesses at one offset: width, s/u for loads (ldrsh vs ldrh), ld/st, count."""
    return ", ".join(f"{w}{'s' if sg else 'u' if sg is False else ''} {'ld' if ld else 'st'}×{n}"
                     for (w, sg, ld), n in sorted(counter.items(), key=lambda x: (x[0][0], str(x[0][1]), x[0][2])))


def short_files(files, n=3):
    files = sorted(files)
    s = ", ".join(os.path.basename(f) for f in files[:n])
    return s + (f" +{len(files) - n}" if len(files) > n else "")


def report(g, addr, entries, rom, syms_sorted, aliases=()):
    """Markdown for one global (direct and pointee spaces)."""
    lines = [f"# {g} ({addr:#010x})", ""]
    if aliases:
        lines += ["Also declared as " + ", ".join(aliases) + " (same address, merged here).", ""]
    for space in ("direct", "pointee"):
        e = entries.get(space)
        rm = rom.get(space, [])
        if not e and not rm:
            continue
        e = e or {"decls": [], "fields": [], "access": [], "sizes": {}}
        an = analyze(g, space, e, addr, rm)
        ext = an["extent"]
        title = "The object" if space == "direct" else f"The pointee (`*{g}`)"
        lines += [f"## {title}", ""]
        types = collections.defaultdict(set)
        for d in e["decls"]:
            types[(d["ctype"], d["how"])].add(d["file"])
        lines.append(f"{len(types)} declared types in {len({d['file'] for d in e['decls']})} files; extent "
                     f"{ext:#x} bytes.")
        if space == "direct":
            inside = [(n, a) for a, n in syms_sorted if addr < a < addr + ext]
            if inside:
                lines.append("Other symbols inside this extent: " +
                             ", ".join(f"{n} (+{a - addr:#x})" for n, a in inside[:12]) +
                             (" ..." if len(inside) > 12 else "") + ".")
        lines.append("")
        lines += ["| type | how | size | files |", "|---|---|---|---|"]
        for (ct, how), files in sorted(types.items(), key=lambda x: -len(x[1])):
            sk = next((d.get("struct") for d in e["decls"] if d["ctype"] == ct and d["how"] == how), None)
            sz = e["sizes"].get(sk) if sk else None
            lines.append(f"| `{ct}` | {how} | {sz:#x} | {short_files(files)} |" if sz else
                         f"| `{ct}` | {how} | | {short_files(files)} |")
        lines.append("")
        # ROM summary
        if rm:
            offs = {a[1] for a in rm}
            funcs = {a[5] for a in rm}
            if space == "pointee":
                lines.append(f"ROM: {len(rm)} accesses at {len(offs)} offsets from {len(funcs)} functions, through "
                             f"the word loaded from {g}.")
            else:
                lines.append(f"ROM: {len(rm)} accesses at {len(offs)} offsets from {len(funcs)} functions. "
                             "Per literal-pool base (one base with several `[rX,#k]` offsets is struct access; "
                             "a literal per field is separately addressed):")
                lines.append("")
                per = collections.defaultdict(collections.Counter)
                for a in rm:
                    per[a[7]][a[8]] += 1
                for lit, ks in sorted(per.items())[:40]:
                    lines.append(f"- literal {g}+{lit - addr:#x}" if lit >= addr else f"- literal {lit:#x}")
                    lines[-1] += (f" ({sum(ks.values())} accesses): #k in " +
                                  ", ".join(f"{k:#x}" for k in sorted(ks)[:24]) + (" ..." if len(ks) > 24 else ""))
                if len(per) > 40:
                    lines.append(f"- ... {len(per) - 40} more literals")
            lines.append("")
        # offset table
        lines += ["### Offset map", "",
                  "| off | size | names (type — files) | opaque in | ROM (width/sign ×n) | C casts |",
                  "|---|---|---|---|---|---|"]
        starts = collections.defaultdict(list)
        for f in e["fields"]:
            starts[f["off"]].append(f)
        romat = collections.defaultdict(collections.Counter)
        for a in rm:
            romat[a[1]][(a[2], a[3], a[4])] += 1
        cat = collections.defaultdict(collections.Counter)
        for a in e["access"]:
            cat[a["off"]][(a["width"], a["signed"], a["form"])] += 1
        conflict_offs = {c[0] for c in an["conflicts"]}
        for off in sorted(set(starts) | set(romat) | set(cat)):
            if off >= ext:
                continue
            fs = starts.get(off, [])
            named = collections.defaultdict(set)
            opq = collections.defaultdict(set)
            size = max([f["size"] for f in fs] or [w for (w, _, _) in romat[off]] or
                       [w for (w, _, _) in cat[off]] or [1])
            for f in fs:
                ty = f["ctype"] + (f":{f['bits']}@{f['bitpos'] % 8}" if f.get("bits") is not None else "")
                (opq if f["opaque"] else named)[(f["name"], ty, f["size"])].update(f["files"])
            nm = "; ".join(f"`{n}` {t} — {short_files(fl, 2)}" for (n, t, s), fl in
                           sorted(named.items(), key=lambda x: -len(x[1]))[:6])
            if len(named) > 6:
                nm += f"; +{len(named) - 6} more"
            op = "; ".join(f"`{n}`[{s:#x}] — {short_files(fl, 2)}" for (n, t, s), fl in
                           sorted(opq.items(), key=lambda x: -len(x[1]))[:3])
            rw = rom_cell(romat[off])
            cw = ", ".join(f"{w}{'s' if sg else 'u' if sg is False else ''} {form}×{n}"
                           for (w, sg, form), n in sorted(cat[off].items(), key=lambda x: str(x[0])))
            flag = " **conflict**" if off in conflict_offs else ""
            unnamed = not an["named"][off] and an["romb"][off]
            if unnamed:
                flag += " **unnamed, used in ROM**"
            lines.append(f"| {off:#x}{flag} | {size:#x} | {nm} | {op} | {rw} | {cw} |")
        lines.append("")
        if an["conflicts"]:
            lines += ["### Conflicts", ""]
            for off, fs, widths, signs in an["conflicts"]:
                desc = "; ".join(sorted({f"`{f['name']}` {f['ctype']} ({short_files(f['files'], 2)})" for f in fs}))
                rw = rom_cell(romat.get(off, {}))
                lines.append(f"- {off:#x}: widths {sorted(widths)}, signed {sorted(map(str, signs))}: {desc}"
                             + (f". ROM: {rw}" if rw else ""))
            lines.append("")
        lines += ["### Opaque in one file, named in another", ""]
        cover = collections.defaultdict(set)  # byte -> names covering it
        for f in e["fields"]:
            if not f["opaque"]:
                for b in range(f["off"], min(f["off"] + f["size"], ext)):
                    cover[b].add(f"`{f['name']}` {f['ctype']}")
        for lo, hi in ranges(an["elsewhere"], key=lambda b: frozenset(cover[b])):
            names = sorted(cover[lo])
            opq_files = set().union(*(an["opq"][b] for b in range(lo, hi)))
            name_files = set().union(*(an["named"][b] for b in range(lo, hi)))
            lines.append(f"- {lo:#x}-{hi:#x}: " + ", ".join(names[:6]) + (" ..." if len(names) > 6 else "")
                         + f" (named in {len(name_files)} files, opaque in {len(opq_files)})")
        if not an["elsewhere"]:
            lines.append("(none)")
        lines += ["", "### Named by no file", ""]
        any_ = False
        for lo, hi in ranges(an["unnamed"]):
            rb = sum(1 for b in range(lo, hi) if an["romb"][b])
            cb = sum(1 for b in range(lo, hi) if an["cacc"][b])
            cov = "opaque" if any(an["covered"][b] for b in range(lo, hi)) else "no declaration"
            lines.append(f"- {lo:#x}-{hi:#x} ({hi - lo:#x} bytes, {cov}): ROM touches {rb} bytes"
                         + (f", C casts {cb}" if cb else ""))
            any_ = True
        if not any_:
            lines.append("(none)")
        lines.append("")
    return "\n".join(lines)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("globals", nargs="*", help="print these globals' reports")
    ap.add_argument("--summary", action="store_true", help="print the ranking table")
    ap.add_argument("-n", type=int, default=40, help="rows in --summary")
    ap.add_argument("--range", help="ADDR-ADDR: only globals in this range")
    ap.add_argument("--no-rom", action="store_true")
    ap.add_argument("--jobs", type=int, default=max(1, (os.cpu_count() or 4) - 2))
    args = ap.parse_args()
    global CHAR_SIGNED
    CHAR_SIGNED = char_signed()
    os.makedirs(OUT, exist_ok=True)
    prune_cache()
    syms = ram_symbols()
    names = sorted(syms)
    results = scan_all(names, args.jobs)
    byaddr = collections.defaultdict(list)
    for n, a in syms.items():
        byaddr[a].append(n)
    alias = {}
    for a, ns in byaddr.items():
        canon = min(ns, key=lambda n: (not n.startswith("gUnk_"), n))
        for n in ns:
            alias[n] = canon
    G, structs, errors = merge(results, alias)
    syms = {n: a for n, a in syms.items() if alias[n] == n}
    names = sorted(syms)

    rom = collections.defaultdict(lambda: collections.defaultdict(list))
    if not args.no_rom and os.path.exists(ELF):
        sorted_syms = sorted((a, n) for n, a in syms.items())
        addrs = [a for a, _ in sorted_syms]
        exts = {}
        for (g, space), e in G.items():
            if space == "direct":
                exts[syms.get(g)] = extent(e)
        for lit, k, w, sg, load, func, ptr in rom_accesses(ELF):
            if lit is None:
                continue
            if ptr:
                for a in (lit,):
                    i = bisect.bisect_right(addrs, a) - 1
                    if i >= 0 and addrs[i] == a:
                        rom[sorted_syms[i][1]]["pointee"].append((lit, k, w, sg, load, func, None, lit, k))
                continue
            x = lit + k
            # every global whose declared extent covers x, else the nearest symbol below
            i = bisect.bisect_right(addrs, x) - 1
            hits = set()
            j = i
            while j >= 0 and addrs[j] > x - 0x10000:
                a = addrs[j]
                if a <= x < a + max(exts.get(a, 0), 1):
                    hits.add(j)
                j -= 1
            if not hits and i >= 0:
                hits.add(i)
            for j in hits:
                a, n = sorted_syms[j]
                rom[n]["direct"].append((a, x - a, w, sg, load, func, None, lit, k))
    else:
        sorted_syms = sorted((a, n) for n, a in syms.items())

    lo, hi = 0, 1 << 32
    if args.range:
        lo, hi = (int(x, 16) for x in args.range.split("-"))
    os.makedirs(OUT, exist_ok=True)
    rows = []
    dump = {}
    for g in names:
        a = syms[g]
        if not lo <= a <= hi:
            continue
        entries = {s: G[(g, s)] for s in ("direct", "pointee") if (g, s) in G}
        if not entries and g not in rom:
            continue
        md = report(g, a, entries, rom.get(g, {}), sorted_syms, [n for n in byaddr[a] if n != g])
        with open(os.path.join(OUT, g + ".md"), "w") as f:
            f.write(md + "\n")
        for space, e in entries.items():
            an = analyze(g, space, e, a, rom.get(g, {}).get(space, []))
            ntypes = len({d["ctype"] for d in e["decls"]})
            opq_bytes = sum(1 for b in range(an["extent"]) if an["opq"][b])
            rows.append({"global": g, "space": space, "addr": a, "types": ntypes,
                         "files": len({d["file"] for d in e["decls"]}), "extent": an["extent"],
                         "opaque": opq_bytes, "elsewhere": len(an["elsewhere"]),
                         "unnamed": len(an["unnamed"]),
                         "unnamed_rom": sum(1 for b in an["unnamed"] if an["romb"][b]),
                         "conflicts": len(an["conflicts"])})
            dump[f"{g}:{space}"] = {"addr": a, "decls": e["decls"], "fields": e["fields"], "access": e["access"],
                                    "rom": [{"off": r[1], "width": r[2], "signed": r[3], "load": r[4],
                                             "func": r[5], "literal": r[7], "k": r[8]}
                                            for r in rom.get(g, {}).get(space, [])]}
    rows.sort(key=lambda r: (-r["elsewhere"], -r["types"], r["addr"]))
    table = ["| global | space | types | files | extent | opaque | opaque, named elsewhere | named nowhere "
             "(ROM-used) | conflicts |", "|---|---|---|---|---|---|---|---|---|"]
    for r in rows:
        table.append(f"| [{r['global']}]({r['global']}.md) | {r['space']} | {r['types']} | {r['files']} | "
                     f"{r['extent']:#x} | {r['opaque']} | {r['elsewhere']} | {r['unnamed']} ({r['unnamed_rom']}) | "
                     f"{r['conflicts']} |")
    with open(os.path.join(OUT, "SUMMARY.md"), "w") as f:
        f.write("# RAM layouts merged across files\n\nRanked by opaque bytes that another file names. "
                "Generated by tools/layoutmerge.py.\n\n" + "\n".join(table) + "\n")
        if errors:
            f.write(f"\n{len(errors)} files could not be scanned:\n\n" +
                    "\n".join(f"- {k}: {v}" for k, v in sorted(errors.items())) + "\n")
    with open(os.path.join(OUT, "layouts.json"), "w") as f:
        json.dump(dump, f)
    multi = len({r["global"] for r in rows if r["types"] > 1})
    print(f"{len(rows)} global layouts ({multi} globals with more than one declared type), "
          f"{len(structs)} struct definitions, {len(errors)} files not scanned; "
          f"reports in {os.path.relpath(OUT, ROOT)}/", file=sys.stderr)
    if args.summary:
        print("\n".join(table[:2 + args.n]))
    for g in args.globals:
        p = os.path.join(OUT, g + ".md")
        print(open(p).read() if os.path.exists(p) else f"{g}: no report")


if __name__ == "__main__":
    main()
