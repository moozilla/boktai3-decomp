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

ROM evidence (build/boktai3.elf, `arm-none-eabi-objdump -d`, Thumb and ARM): a
linear scan of each function tracks registers loaded from literal pools. An
access `[rX, #k]` through a RAM literal L is a typed access at L+k; ldm/stm,
BIOS/libc copies and fills with a constant length, and DMA register writes are
bulk traffic; an address passed to a call or stored is an escape. A word loaded
from a global and then dereferenced gives accesses to that pointer's pointee.
In agbcc's Thumb code one literal plus several offsets is a struct, and one
literal per field is separate globals; the report lists both.

Objects are keyed by address. A symbol inside another object's extent (a
palette label inside a 512-byte buffer, &records[3] under its own name) folds
into it. Every byte gets one observation class: typed and named, typed but
unnamed, bulk only, escape only, or unobserved (never "unused").

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
                d["array"], d["count"] = True, len(probes)
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
            if k in d:  # sizeof of an incomplete array: 0 or refused
                d[k] = vals[d[k]] or None
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
POOL = re.compile(r"@ \(?([0-9a-f]+)")
REG = {f"r{i}": i for i in range(13)}
REG.update(sb=9, sl=10, fp=11, ip=12)
WIDTH = {"ldr": (4, None), "str": (4, None), "ldrh": (2, False), "strh": (2, None), "ldrb": (1, False),
         "strb": (1, None), "ldrsh": (2, True), "ldrsb": (1, True)}
COND = re.compile(r"^(\w+?)(eq|ne|cs|cc|hs|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le)$")
DMA = range(0x040000B0, 0x040000E0)  # DMA0-3 SAD/DAD/CNT


class Acc(collections.namedtuple("Acc", "lit k width signed load func ptr form note pc")):
    """One ROM observation at literal `lit` + k (ptr: through the word stored at lit).
    form: "typed" (ldr*/str*), "bulk" (ldm/stm, BIOS/libc copy, DMA), "escape" (address
    passed to a call or stored)."""


def bulk_call(callee, st):
    """(dest/src reg, byte length) ranges a call is known to copy or fill."""
    def c(r):
        v = st.get(r)
        return v[1] if v and v[0] == "const" else None
    if callee in ("CpuSet", "CpuFastSet") and c(2) is not None:
        ctrl = c(2)
        n = ctrl & 0x1FFFFF
        n = n * 4 if callee == "CpuFastSet" or ctrl >> 26 & 1 else n * 2
        return [(0, 4 if ctrl >> 24 & 1 else n), (1, n)]  # a fill reads one unit
    if callee in ("memcpy", "Memory_Copy") and c(2) is not None:
        return [(0, c(2)), (1, c(2))]
    if callee == "memset" and c(2) is not None:
        return [(0, c(2))]
    if callee == "Memory_Clear" and c(1) is not None:
        return [(0, c(1))]
    return None


def rom_accesses(elf_path):
    """Acc records from every function in the code (Thumb and ARM)."""
    data = open(elf_path, "rb").read()
    shoff, = pystruct.unpack_from("<I", data, 0x20)
    shnum, = pystruct.unpack_from("<H", data, 0x30)
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
            if not m.group(2).startswith("_0"):  # _0800xxxx are local labels, not functions
                if func:
                    Scan(func, body, word, out).run()
                func, body = m.group(2).removesuffix("__addr"), []
            body.append(("label", int(m.group(1), 16)))
            continue
        m = INSN.match(s)
        if m:
            body.append((int(m.group(1), 16), m.group(2), m.group(3)))
    if func:
        Scan(func, body, word, out).run()
    return out


class Scan:
    """Linear register tracking through one function. A register holds
    ("addr", A): a RAM address; ("ptr", A, k): the word loaded from A, plus k;
    ("const", v); or nothing known. State ends at unconditional branches; at a
    branch target the fall-through state is kept (a heuristic)."""

    def __init__(self, func, body, word, out):
        self.func, self.body, self.word, self.out = func, body, word, out
        self.st, self.based = {}, set()  # based: registers used as a memory base since they were set
        self.dma = {}

    def set(self, r, v):
        if r is not None:
            self.st[r] = v
            self.based.discard(r)

    def rec(self, b, k, w, sg, load, form, note=None):
        if b and b[0] == "addr":
            self.out.append(Acc(b[1], k, w, sg, load, self.func, False, form, note, self.pc))
        elif b and b[0] == "ptr":
            self.out.append(Acc(b[1], b[2] + k, w, sg, load, self.func, True, form, note, self.pc))

    def run(self):
        pools = set()
        for it in self.body:
            if it[0] != "label" and it[1].startswith("ldr") and "[pc" in it[2]:
                m = POOL.search(it[2])
                if m:
                    pools.update(range(int(m.group(1), 16), int(m.group(1), 16) + 4))
        flow = True
        for it in self.body:
            if it[0] == "label":
                if not flow:
                    self.st, self.based = {}, set()
                continue
            self.pc, op, args = it
            if self.pc in pools:
                self.st, flow = {}, False
                continue
            flow = True
            try:
                flow = self.step(op, args)
            except (ValueError, TypeError, KeyError, IndexError, AttributeError):
                ops = args.split(",")
                self.set(REG.get(ops[0].strip()), None)

    def step(self, op, args):
        st = self.st
        cond = False
        m = COND.match(op)
        if m and m.group(1) in ("ldr", "str", "ldrh", "strh", "ldrb", "strb", "ldrsh", "ldrsb", "mov", "add",
                                "sub", "b", "bx", "bl", "pop", "ldm", "stm", "lsl", "cmp"):
            op, cond = m.group(1), True  # ARM conditional execution
        text = args.split("@")[0].strip()
        ops = [x.strip() for x in re.split(r",(?![^\[{]*[\]}])", text)] if text else []
        dest = REG.get(ops[0].rstrip("!")) if ops else None
        if op == "ldr" and "[pc" in text:
            m = POOL.search(args)
            v = self.word(int(m.group(1), 16)) if m else None
            self.set(dest, None if cond or v is None else ("addr", v) if is_ram(v) else ("const", v))
            return True
        if op in WIDTH and len(ops) >= 2 and ops[1].startswith("["):
            return self.mem(op, ops, dest, cond)
        if op in ("ldmia", "stmia", "ldm", "stm", "ldmfd", "stmfd", "ldmea", "stmea"):
            rn = REG.get(ops[0].rstrip("!"))
            b = st.get(rn)
            self.based.add(rn)
            regs = [REG[r] for r in re.findall(r"\b(r\d+|ip|lr|fp|sl|sb)\b", text.split("{", 1)[1]) if r in REG]
            for i in range(len(regs)):
                self.rec(b, 4 * i, 4, None, op.startswith("ldm"), "bulk")
            if op.startswith("ldm"):
                for r in regs:
                    self.set(r, None)
            elif b and b[0] == "addr":
                for i, r in enumerate(regs):  # storing an address is an escape
                    v = st.get(r)
                    if v and v[0] == "addr" and r not in self.based:
                        self.rec(v, 0, 0, None, False, "escape", "stored")
            if ops[0].endswith("!"):
                self.set(rn, shift(b, 4 * len(regs)))
            return True
        if op in ("movs", "mov", "mvns", "mvn") and len(ops) == 2:
            if cond:
                self.set(dest, None)
            elif ops[1].startswith("#"):
                v = int(ops[1][1:], 0)
                self.set(dest, ("const", v if op.startswith("mov") else ~v & 0xFFFFFFFF))
            elif op.startswith("mov"):
                src = REG.get(ops[1])
                self.set(dest, st.get(src))
                if src in self.based:
                    self.based.add(dest)
            else:
                self.set(dest, None)
            return True
        if op in ("adds", "add", "subs", "sub") and dest is not None and dest < 13:
            sign = -1 if op.startswith("sub") else 1
            if len(ops) == 2:  # adds rd, #imm / add rd, rm
                src, other = st.get(dest), ops[1]
            else:
                src, other = st.get(REG.get(ops[1])), ops[2]
            if cond or len(ops) > 3:  # ARM shifted operand
                v = None
            elif other.startswith("#"):
                v = shift(src, sign * int(other[1:], 0))
            else:
                o = st.get(REG.get(other))
                if src and o and sign == 1 and (src[0] == "const") != (o[0] == "const"):
                    p, c = (o, src) if src[0] == "const" else (src, o)
                    v = shift(p, c[1])
                elif src and o and src[0] == o[0] == "const":
                    v = ("const", (src[1] + sign * o[1]) & 0xFFFFFFFF)
                else:
                    v = None
            self.set(dest, v)
            return True
        if op in ("lsls", "lsl") and len(ops) == 3 and ops[2].startswith("#"):
            s0 = st.get(REG.get(ops[1]))
            self.set(dest, ("const", (s0[1] << int(ops[2][1:], 0)) & 0xFFFFFFFF)
                     if s0 and s0[0] == "const" and not cond else None)
            return True
        if op in ("bl", "blx"):
            m = re.search(r"<([^>+]+)>", args)
            callee = m.group(1).removesuffix("__addr") if m else "?"
            ranges = dict(bulk_call(callee, st) or [])
            for r in (0, 1, 2, 3):
                v = st.get(r)
                if v and v[0] in ("addr", "ptr"):
                    if r in ranges and ranges[r]:
                        n = ranges[r]
                        for k in range(0, min(n, 0x10000), 4):  # one record per word keeps counts readable
                            self.rec(v, k, min(4, n - k), None, r != 0 or callee == "?", "bulk", callee)
                    elif r not in self.based:
                        self.rec(v, 0, 0, None, False, "escape", callee)
            for r in (0, 1, 2, 3, 12, 14):
                self.set(r, None)
            return True
        if op in ("b", "b.n", "bx") or (op == "pop" and "pc" in text) or op == "ldmfd" and "pc" in text:
            if not cond:
                self.st, self.based = {}, set()
                return False
            return True
        if op == "pop":
            for r in re.findall(r"r\d+", text):
                self.set(REG[r], None)
            return True
        if op.startswith(("b", "cmp", "cmn", "tst", "teq", "push", "svc", "swi", "str")):
            return True
        self.set(dest, None)
        return True

    def mem(self, op, ops, dest, cond):
        """ldr*/str* rd, [rn, #k | rm] (Thumb), plus ARM pre/post-index forms."""
        st = self.st
        inner = [x.strip() for x in ops[1].rstrip("!").strip("[]").split(",")]
        rn = REG.get(inner[0])
        b, k = st.get(rn), 0
        self.based.add(rn)
        if len(inner) > 1:
            if inner[1].startswith("#"):
                k = int(inner[1][1:], 0)
            else:
                o = st.get(REG.get(inner[1]))
                if o and o[0] == "const" and b and b[0] != "const":
                    k = o[1]
                elif o and o[0] != "const" and b and b[0] == "const":
                    b, k = o, b[1]
                    self.based.add(REG.get(inner[1]))
                else:
                    b = None
        w, sg = WIDTH[op]
        load = op.startswith("ldr")
        self.rec(b, k, w, sg, load, "typed")
        if not load:
            v = st.get(dest)
            to_dma = b and b[0] == "const" and b[1] + k in DMA
            if v and v[0] == "addr" and dest not in self.based and not to_dma:  # an address stored to memory
                self.rec(v, 0, 0, None, False, "escape", "stored")
            reg = b[1] + k if b and b[0] == "const" else None
            if reg in DMA:  # DMAn SAD +0, DAD +4, CNT +8; the CNT write starts the transfer
                ch, slot = divmod(reg - DMA.start, 12)
                if slot < 8 and v and v[0] in ("addr", "ptr"):
                    self.dma.setdefault(ch, {})[slot] = v
                c = st.get(dest)
                if slot == 8 and c and c[0] == "const":
                    n = (c[1] & 0xFFFF or 0x10000) * (4 if c[1] >> 26 & 1 else 2)
                    for slot, v in self.dma.pop(ch, {}).items():
                        fixed = (c[1] >> (23 if slot == 0 else 21) & 3) == 2
                        for kk in range(0, min(n if not fixed else 4, 0x10000), 4):
                            self.rec(v, kk, 4, None, slot == 0, "bulk", "DMA")
        if ops[1].endswith("!") or len(ops) == 3:  # ARM writeback
            post = int(ops[2][1:], 0) if len(ops) == 3 and ops[2].startswith("#") else None
            self.set(rn, shift(b, k) if ops[1].endswith("!") else shift(b, post) if post is not None else None)
        if load:
            self.set(dest, None if cond else ("ptr", b[1] + k, 0) if b and b[0] == "addr" and w == 4 else None)
        return True


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


def opaque(name, lf=None, letters=False):
    """A member nobody has named: unk/pad/field..., a letter plus an offset
    (f54), a byte array named by one letter (u8 f[0x868]), or any one-letter
    name in a struct that names its members a, b, c, ... (letters=True)."""
    own = re.split(r"[\[]", name.rsplit(".", 1)[-1])[0]
    if OPAQUE.match(own) or PLACEHOLDER.match(own) or (letters and len(own) == 1):
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
    G = collections.defaultdict(lambda: {"decls": [], "fields": [], "access": [], "sizes": {}, "rom": []})
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
    letters = {key: {"a", "b", "c"} <= {lf["name"] for lf in st["leaves"]} for key, st in structs.items()}
    for (g, space), e in G.items():
        users = collections.defaultdict(set)  # (struct key, count, unsized) -> files using it here
        for d in e["decls"]:
            if d.get("struct") in structs:
                unsized = bool(d.get("array")) and not d.get("count")
                users[(d["struct"], d.get("count") or 1, unsized)].add(d["file"])
        for (skey, count, unsized), files in users.items():
            st = structs[skey]
            stride = st["size"] or 0
            if not stride or count * stride > 0x10000:
                count = 1
            for i in range(count):
                for off, size, lf, name in placed[skey]:
                    nm = f"[{i}].{name}" if count > 1 or unsized else name
                    e["fields"].append({
                        "off": i * stride + off, "size": size, "name": nm,
                        "ctype": lf["ctype"], "signed": lf["signed"], "base": lf["base"], "array": lf["array"],
                        "width": lf.get("esize") if lf["array"] else
                        size if lf.get("bits") is not None else lf.get("width") or size,
                        "bits": lf.get("bits"), "bitpos": lf.get("bitpos"), "struct": skey, "spell": st["spell"],
                        "files": sorted(files), "opaque": opaque(name, lf, letters[skey]),
                        "repeat": stride if unsized and stride else None})
            e["sizes"][skey] = (st["size"] or 0) * count if count > 1 else st["size"]
        if space == "direct":  # a scalar names its bytes; an array view (s16 g[], u8 g[]) names no field
            scal = collections.defaultdict(set)
            for d in e["decls"]:
                if "struct" not in d and d.get("width"):
                    size = d.get("sizeof") or (None if d["array"] else d["width"])  # None: g[], to the extent
                    scal[(d["ctype"], size, d["width"], d["signed"], d["base"], d["array"])].add(d["file"])
            for (ct, size, w, sg, base, arr), files in scal.items():
                e["fields"].append({"off": 0, "size": size, "name": g, "ctype": ct, "signed": sg, "base": base,
                                    "array": arr, "width": w, "bits": None, "bitpos": None, "struct": None,
                                    "spell": ct, "files": sorted(files), "opaque": arr,
                                    "repeat": None})
    return G, structs, errors


def extent(e, rom=True):
    """How far the object reaches: declared sizes, fields, C accesses, and (rom)
    typed ROM accesses. Unsized arrays don't count; they reach this far. Nor does
    bulk traffic: one clear often covers many objects."""
    ext = max([s for s in e["sizes"].values() if s] + [0])
    for f in e["fields"]:
        if f["size"] is not None and not f.get("to_extent"):
            ext = max(ext, f["off"] + f["size"])
    for a in e["access"]:
        ext = max(ext, a["off"] + a["width"])
    if rom:
        runs = collections.defaultdict(list)  # bulk transfers by call site
        for a in e["rom"]:
            if a["form"] == "typed":
                ext = max(ext, a["off"] + a["width"])
            elif a["form"] == "bulk":
                runs[a.get("pc")].append(a)
        for run in runs.values():  # a copy or fill that starts at the object and isn't a region clear
            lo, hi = min(a["off"] for a in run), max(a["off"] + a["width"] for a in run)
            if lo == 0 and hi <= 0x1000:
                ext = max(ext, hi)
    return min(ext, 0x10000)


def fold_interiors(G, syms):
    """Fold the declarations of symbols inside an object into it: palette labels
    inside a 512-byte buffer, &records[3] named as its own symbol. Inside means
    within the object's extent; for an unsized array of S-byte structs, a symbol
    at g + j*S declared with an S-byte struct (never an array of structs) is element j
    and extends the array.
    Returns {interior: (container, offset)}."""
    order = sorted((a, n) for n, a in syms.items())
    addrs = [a for a, _ in order]
    orig = {g: {k: type(v)(v) for k, v in e.items()}
            for (g, sp), e in G.items() if sp == "direct"}
    parent = {}
    for (g, space), e in list(G.items()):
        if space != "direct":
            continue
        base = syms.get(g)
        if base is None:
            continue
        ext = extent(e)
        strides = {f["repeat"] for f in e["fields"] if f.get("repeat")}
        i = bisect.bisect_right(addrs, base)
        while i < len(order):
            a, h = order[i]
            delta = a - base
            hs = orig.get(h)
            if delta >= ext:  # past the end: only an element alias extends an unsized array
                single = set(hs["sizes"].values()) if hs and not any(
                    d.get("array") and d.get("struct") for d in hs["decls"]) else set()
                fit = {s for s in strides & single if delta % s == 0 and delta < 256 * s}
                if not fit:
                    break
                ext = max(ext, delta + max(fit))
            i += 1
            if not hs:
                continue
            parent.setdefault(h, (g, delta))
            for d in hs["decls"]:
                e["decls"].append(dict(d, how=f"via {h} +{delta:#x}", via=h))
            for f in hs["fields"]:
                nm = h if f["name"] == h else f"{h}.{f['name']}"
                e["fields"].append(dict(f, off=f["off"] + delta, name=nm, via=h, repeat=None,
                                        size=f["size"] if f["size"] is not None else f["width"]))
            for c in hs["access"]:
                e["access"].append(dict(c, off=c["off"] + delta, via=h))
            for skey, s in hs["sizes"].items():
                if s:
                    e["sizes"][f"{skey}@{h}"] = delta + s
    return parent


def expand(e, ext):
    """Fields with unsized arrays reaching ext: g[] scalars cover it, g[] structs repeat."""
    out = []
    for f in e["fields"]:
        if f["size"] is None or f.get("to_extent"):
            out.append(dict(f, size=max(ext - f["off"], f["width"] or 1), to_extent=True))
        elif f.get("repeat"):
            s = f["repeat"]
            n = min(max(1, -(-(ext - f["off"]) // s)), 4096)
            out += [dict(f, off=f["off"] + j * s, name=re.sub(r"^\[0\]", f"[{j}]", f["name"])) for j in range(n)]
        else:
            out.append(f)
    return out


CLASSES = ["typed, named", "typed, meaning unresolved", "bulk only", "address escape only", "unobserved"]


def analyze(e):
    """Byte map, conflicts, observation classes for one global/space."""
    ext = extent(e)
    fields = expand(e, ext)
    named = [set() for _ in range(ext)]  # files that name this byte
    opq = [set() for _ in range(ext)]
    starts = set()
    for f in fields:
        tgt = opq if f["opaque"] else named
        starts.add(f["off"])
        for b in range(f["off"], min(f["off"] + f["size"], ext)):
            tgt[b].update(f["files"])
    typed, bulk, escape = [0] * ext, [0] * ext, [0] * ext
    for a in e["rom"]:
        if a["form"] == "escape":
            if a["off"] < ext:
                escape[a["off"]] += 1
            continue
        # a word copy across two declared fields is bulk traffic, not a field
        spans = any(s in starts for s in range(a["off"] + 1, a["off"] + a["width"]))
        tgt = bulk if a["form"] == "bulk" or spans else typed
        for b in range(a["off"], min(a["off"] + a["width"], ext)):
            tgt[b] += 1
    for a in e["access"]:
        for b in range(a["off"], min(a["off"] + a["width"], ext)):
            typed[b] += 1
    cls = []
    for b in range(ext):
        if typed[b]:
            cls.append(0 if named[b] else 1)
        else:
            cls.append(2 if bulk[b] else 3 if escape[b] else 4)
    elsewhere = [b for b in range(ext) if opq[b] and named[b] - opq[b]]
    unnamed = [b for b in range(ext) if not named[b]]
    by_off = collections.defaultdict(list)
    for f in fields:
        if not f["opaque"] and f["base"] in ("int", "ptr", "enum") and f.get("bits") is None:
            by_off[f["off"]].append(f)
    conflicts = []
    for off, fs in sorted(by_off.items()):
        widths = {f["width"] for f in fs if f["width"]}
        signs = {f["signed"] for f in fs if f["signed"] is not None and f["base"] == "int"}
        if len(widths) > 1 or len(signs) > 1:
            conflicts.append((off, fs, widths, signs))
    return {"extent": ext, "fields": fields, "named": named, "opq": opq, "typed": typed, "bulk": bulk,
            "escape": escape, "cls": cls, "elsewhere": elsewhere, "unnamed": unnamed, "conflicts": conflicts}


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


DEF = re.compile(r"^(?!\s)(?:[\w*]+[ \t*]+)+\**(\w+)\s*\([^;{}]*\)\s*\{", re.M)
INCASM = re.compile(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)')
ALIAS = re.compile(r"^#define\s+(\w+)\s+(\w+)\s*$", re.M)


def function_files():
    """Function name -> the C files that define it (units first, then drafts)."""
    out = collections.defaultdict(set)
    for f in sources():
        text = open(os.path.join(ROOT, f), errors="replace").read()
        names = set(DEF.findall(text)) | set(INCASM.findall(text))
        for a, b in ALIAS.findall(text):
            if a in names or b in names:
                names |= {a, b}
        for n in names:
            out[n].add(f)
    return {n: sorted(fs, key=lambda f: (f.startswith("src/fn/"), f)) for n, fs in out.items()}


def report(g, addr, entries, syms_sorted, aliases=(), parent=None, funcs=None):
    """Markdown for one global (direct and pointee spaces)."""
    lines = [f"# {g} ({addr:#010x})", ""]
    if aliases:
        lines += ["Also declared as " + ", ".join(aliases) + " (same address, merged here).", ""]
    if parent:
        lines += [f"Inside {parent[0]} at +{parent[1]:#x}: its report merges this symbol's declarations "
                  "with the containing object's.", ""]
    for space in ("direct", "pointee"):
        e = entries.get(space)
        if not e:
            continue
        an = analyze(e)
        ext, fields = an["extent"], an["fields"]
        rm = e["rom"]
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
                lines.append("Symbols inside this extent: " + ", ".join(f"{n} (+{a - addr:#x})" for n, a in inside[:16])
                             + (" ..." if len(inside) > 16 else "") + ".")
        lines += ["", "| type | how | size | files |", "|---|---|---|---|"]
        for (ct, how), files in sorted(types.items(), key=lambda x: (x[0][1].startswith("via"), -len(x[1]))):
            sk = next((d.get("struct") for d in e["decls"] if d["ctype"] == ct and d["how"] == how), None)
            sz = e["sizes"].get(sk) if sk else None
            lines.append(f"| `{ct}` | {how} | {f'{sz:#x}' if sz else ''} | {short_files(files)} |")
        lines.append("")
        typed = [a for a in rm if a["form"] == "typed"]
        if rm:
            forms = collections.Counter(a["form"] for a in rm)
            fn = {a["func"] for a in rm}
            lines.append(f"ROM: {forms['typed']} typed accesses, {forms['bulk']} words of bulk traffic, "
                         f"{forms['escape']} address escapes, from {len(fn)} functions"
                         + (f", through the word loaded from {g}." if space == "pointee" else "."))
            if space == "direct" and typed:
                lines += ["", "Typed accesses per literal-pool base (one base used with several `[rX,#k]` "
                          "offsets is struct access; a literal per field is separately addressed):", ""]
                per = collections.defaultdict(collections.Counter)
                for a in typed:
                    per[a["lit"]][a["k"]] += 1
                for lit, ks in sorted(per.items())[:40]:
                    where = f"{g}+{lit - addr:#x}" if lit >= addr else f"{lit:#x}"
                    lines.append(f"- literal {where} ({sum(ks.values())}): #k in "
                                 + ", ".join(f"{k:#x}" for k in sorted(ks)[:24]) + (" ..." if len(ks) > 24 else ""))
                if len(per) > 40:
                    lines.append(f"- ... {len(per) - 40} more literals")
            esc = collections.defaultdict(collections.Counter)
            for a in rm:
                if a["form"] in ("escape", "bulk"):
                    esc[(a["form"], a["note"] or "ldm/stm")][a["off"] if a["form"] == "escape" else 0] += 1
            if esc:
                lines += ["", "Bulk traffic and escapes by callee: " + "; ".join(
                    f"{note} ({form}" + (", at " + ", ".join(f"+{o:#x}" for o in sorted(c)[:6]) if form == "escape"
                                         else "") + f") ×{sum(c.values())}"
                    for (form, note), c in sorted(esc.items(), key=lambda x: -sum(x[1].values()))[:12]) + "."]
            lines.append("")
        # offset map
        lines += ["### Offset map", "",
                  "| off | size | names (type — files) | opaque in | ROM typed (width/sign ×n) | C casts |",
                  "|---|---|---|---|---|---|"]
        starts = collections.defaultdict(list)
        for f in fields:
            starts[f["off"]].append(f)
        romat = collections.defaultdict(collections.Counter)
        for a in typed:
            romat[a["off"]][(a["width"], a["signed"], a["load"])] += 1
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
            cw = ", ".join(f"{w}{'s' if sg else 'u' if sg is False else ''} {form}×{n}"
                           for (w, sg, form), n in sorted(cat[off].items(), key=lambda x: str(x[0])))
            flag = " **conflict**" if off in conflict_offs else ""
            if not an["named"][off] and an["typed"][off]:
                flag += " **unnamed, typed access**"
            lines.append(f"| {off:#x}{flag} | {size:#x} | {nm} | {op} | {rom_cell(romat[off])} | {cw} |")
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
        for f in fields:
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
        lines += ["", "### What the evidence says about each byte", "",
                  "Typed: a ROM load/store or C cast of the field's width; bulk: ldm/stm, a BIOS/libc copy or "
                  "fill, DMA, or one access spanning two declared fields; escape: the address is passed to a call "
                  "or stored. Unobserved means no access in this scan, not unused.", ""]
        for lo, hi in ranges(range(ext), key=lambda b: an["cls"][b]):
            c = an["cls"][lo]
            nm = "named" if any(an["named"][b] for b in range(lo, hi)) else "not named by any file"
            lines.append(f"- {lo:#x}-{hi:#x} ({hi - lo:#x}): {CLASSES[c]}" + ("" if c < 2 else f"; {nm}"))
        lines.append("")
        lines += ["### Consumers", "",
                  "Functions whose code touches this object, and where their C is. A shared declaration "
                  "must keep each of them byte-exact (`tools/check.py`) before it replaces the per-file ones.", ""]
        byf = collections.defaultdict(collections.Counter)
        for a in rm:
            byf[a["func"]][a["form"]] += 1
        for fn, c in sorted(byf.items(), key=lambda x: -sum(x[1].values()))[:60]:
            files = (funcs or {}).get(fn, [])
            lines.append(f"- {fn}: " + ", ".join(f"{k} {v}" for k, v in sorted(c.items()))
                         + (f" — {files[0]}" + (f" (+{len(files) - 1})" if len(files) > 1 else "") if files else ""))
        if len(byf) > 60:
            lines.append(f"- ... {len(byf) - 60} more functions")
        if not byf:
            lines.append("(no ROM accesses found)")
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
    results = scan_all(sorted(syms), args.jobs)
    byaddr = collections.defaultdict(list)
    for n, a in syms.items():
        byaddr[a].append(n)
    alias = {n: min(ns, key=lambda n: (not n.startswith("gUnk_"), n)) for ns in byaddr.values() for n in ns}
    G, structs, errors = merge(results, alias)
    syms = {n: a for n, a in syms.items() if alias[n] == n}
    sorted_syms = sorted((a, n) for n, a in syms.items())
    addrs = [a for a, _ in sorted_syms]
    acc = [] if args.no_rom or not os.path.exists(ELF) else rom_accesses(ELF)
    # a symbol's own literal says how far its object reaches, before anything is folded in
    for a in acc:
        if not a.ptr and a.form != "escape":
            i = bisect.bisect_right(addrs, a.lit) - 1
            if i >= 0 and addrs[i] == a.lit:
                G[(sorted_syms[i][1], "direct")]["rom"].append({"off": a.k, "width": a.width, "form": a.form,
                                                                "pc": a.pc})
    parent = fold_interiors(G, syms)
    exts = {syms[g]: extent(e) for (g, sp), e in G.items() if sp == "direct" and g in syms}
    for e in G.values():
        e["rom"] = []
    for a in acc:
        rec = {"width": a.width, "signed": a.signed, "load": a.load, "func": a.func, "form": a.form,
               "note": a.note, "lit": a.lit, "k": a.k, "pc": a.pc}
        if a.ptr:
            i = bisect.bisect_right(addrs, a.lit) - 1
            if i >= 0 and addrs[i] == a.lit:
                G[(sorted_syms[i][1], "pointee")]["rom"].append(dict(rec, off=a.k))
            continue
        x = a.lit + a.k
        # every object whose extent covers x, else the nearest symbol below
        i = bisect.bisect_right(addrs, x) - 1
        hits, j = set(), i
        while j >= 0 and addrs[j] > x - 0x10000:
            if addrs[j] <= x < addrs[j] + max(exts.get(addrs[j], 0), 1):
                hits.add(j)
            j -= 1
        if not hits and i >= 0:
            hits.add(i)
        for j in hits:
            G[(sorted_syms[j][1], "direct")]["rom"].append(dict(rec, off=x - addrs[j]))

    lo, hi = (int(x, 16) for x in args.range.split("-")) if args.range else (0, 1 << 32)
    funcs = function_files()
    rows, dump = [], {}
    for g in sorted(syms):
        a = syms[g]
        if not lo <= a <= hi:
            continue
        entries = {s: G[(g, s)] for s in ("direct", "pointee")
                   if (g, s) in G and (G[(g, s)]["decls"] or G[(g, s)]["rom"])}
        if not entries:
            continue
        md = report(g, a, entries, sorted_syms, [n for n in byaddr[a] if n != g], parent.get(g), funcs)
        with open(os.path.join(OUT, g + ".md"), "w") as f:
            f.write(md + "\n")
        for space, e in entries.items():
            an = analyze(e)
            cls = collections.Counter(an["cls"])
            rows.append({"global": g, "space": space, "addr": a, "types": len({d["ctype"] for d in e["decls"] if not d.get("via")}),
                         "files": len({d["file"] for d in e["decls"] if not d.get("via")}), "extent": an["extent"],
                         "opaque": sum(1 for b in range(an["extent"]) if an["opq"][b]),
                         "elsewhere": len(an["elsewhere"]), "unnamed": len(an["unnamed"]),
                         "typed_unnamed": cls[1], "bulk_escape": cls[2] + cls[3], "unobserved": cls[4],
                         "conflicts": len(an["conflicts"]), "inside": parent.get(g, (None,))[0]})
            dump[f"{g}:{space}"] = {"addr": a, "extent": an["extent"], "inside": parent.get(g),
                                    "decls": e["decls"], "fields": an["fields"], "access": e["access"],
                                    "rom": e["rom"],
                                    "classes": [[lo_, hi_, CLASSES[an["cls"][lo_]]] for lo_, hi_ in
                                                ranges(range(an["extent"]), key=lambda b: an["cls"][b])]}
    rows.sort(key=lambda r: (-r["elsewhere"], -r["types"], r["addr"]))
    table = ["| global | space | types | files | extent | opaque | opaque, named elsewhere | typed, unnamed "
             "| bulk/escape only | unobserved | conflicts |", "|---|---|---|---|---|---|---|---|---|---|---|"]
    for r in rows:
        inside = f" (in {r['inside']})" if r["inside"] else ""
        table.append(f"| [{r['global']}]({r['global']}.md){inside} | {r['space']} | {r['types']} | {r['files']} | "
                     f"{r['extent']:#x} | {r['opaque']} | {r['elsewhere']} | {r['typed_unnamed']} | "
                     f"{r['bulk_escape']} | {r['unobserved']} | {r['conflicts']} |")
    with open(os.path.join(OUT, "SUMMARY.md"), "w") as f:
        f.write("# RAM layouts merged across files\n\nRanked by opaque bytes that another file names. "
                "Generated by tools/layoutmerge.py.\n\n" + "\n".join(table) + "\n")
        if errors:
            f.write(f"\n{len(errors)} files could not be scanned:\n\n" +
                    "\n".join(f"- {k}: {v}" for k, v in sorted(errors.items())) + "\n")
    with open(os.path.join(OUT, "layouts.json"), "w") as f:
        json.dump(dump, f)
    multi = len({r["global"] for r in rows if r["types"] > 1})
    print(f"{len(rows)} global layouts ({multi} globals with more than one declared type, {len(parent)} folded "
          f"into a containing object), {len(structs)} struct definitions, {len(errors)} files not scanned; "
          f"reports in {os.path.relpath(OUT, ROOT)}/", file=sys.stderr)
    if args.summary:
        print("\n".join(table[:2 + args.n]))
    for g in args.globals:
        p = os.path.join(OUT, g + ".md")
        print(open(p).read() if os.path.exists(p) else f"{g}: no report")


if __name__ == "__main__":
    main()
