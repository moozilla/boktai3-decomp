#!/usr/bin/env python3
"""Write raw byte offsets into typed objects as member accesses, keeping each on MATCH.

  rawfield.py [PATH ...] [--skip build/skip_files.txt] [--dry-run] [--report FILE]

A draft written before a struct had its members reaches into it by byte offset:

    *(u16 *)((u8 *)actor + 0x2D0)            ->  actor->statusFlags
    *((u8 *)p + 0xAF)                        ->  p->mode
    (struct Seq *)((u8 *)p + 0x48)           ->  &p->sequence
    ((struct Widget *)((u8 *)s + 0x25C0))->flags  ->  s->widget.flags
    Collider_Remove((u8 *)owner + 0x120)     ->  Collider_Remove(&owner->collider)
    (u8 *)&gState + 0x10                     ->  &gState.field10

The base must be a plain variable whose declared type is a struct or union: a parameter or local
declared `struct T *p` (or through a typedef) in the same function, or a global declared in the
unit or its headers (`&g` for an object, `g` for a pointer). Its layout comes from the compiler
(tools/layoutmerge.py's probe, the one tools/struct_views.py uses), so packing and nested
members are exact. A member qualifies when it starts at exactly that offset (a nested member or
an element of a scalar array counts) and its type agrees with the cast:

* an integer cast needs a member of the same width and signedness, not a bitfield;
* any other cast (a pointer, a struct, a typedef) needs agbcc to accept `T *x = &member`
  without a warning, checked in one probe compile per unit;
* `(void *)` and a bare `(u8 *)p + K` passed on take the outermost member there (a `u8 *`
  destination wants a u8 member, which the warning check enforces).

Each rewrite stays only when tools/check.py still reports MATCH with no new compiler warning
(tools/keepcheck.py). Code inside `#ifdef NONMATCHING` is never touched (check.py does not
compile it), nor are macros. Sites the tool understands but cannot apply (no member at the
offset, a member of another type, a mismatch) are reported for review.
"""
from __future__ import annotations

import argparse
import collections
import os
import re
import subprocess
import sys
import tempfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, TOOLS)
import keepcheck  # noqa: E402
from keepcheck import ROOT  # noqa: E402

NUM = r"(0[xX][0-9A-Fa-f]+|\d+)"
TYPE = r"(?:const\s+|volatile\s+)*((?:struct\s+|union\s+)?\w+(?:\s*\*)*?)\s*(?:const\s+)?\*\s*"
# (u8 *)V + K and (u8 *)&V + K, with V a plain identifier
CORE = r"\(\s*(?:const\s+)?u8\s*\*\s*\)\s*(&\s*)?([A-Za-z_]\w*)\s*\+\s*" + NUM
FORMS = [
    # ((T *)((u8 *)V + K))->f
    ("arrow", re.compile(r"\(\s*\(\s*" + TYPE + r"\)\s*\(\s*" + CORE + r"\s*\)\s*\)\s*->\s*([A-Za-z_]\w*)")),
    # *(T *)((u8 *)V + K)
    ("deref", re.compile(r"\*\s*\(\s*" + TYPE + r"\)\s*\(\s*" + CORE + r"\s*\)")),
    # *((u8 *)V + K)
    ("deref8", re.compile(r"\*\s*\(\s*" + CORE + r"\s*\)")),
    # (T *)((u8 *)V + K)
    ("addr", re.compile(r"\(\s*" + TYPE + r"\)\s*\(\s*" + CORE + r"\s*\)")),
    # (u8 *)V + K
    ("bare", re.compile(CORE)),
]
INTS = {"u8": (1, False), "s8": (1, True), "u16": (2, False), "s16": (2, True), "u32": (4, False),
        "s32": (4, True), "vu8": (1, False), "vs8": (1, True), "vu16": (2, False), "vs16": (2, True),
        "vu32": (4, False), "vs32": (4, True), "bool8": (1, False), "bool16": (2, False),
        "bool32": (4, False), "int": (4, True), "unsigned": (4, False), "short": (2, True), "char": (1, False)}
SCALARS = set(INTS) | {"void", "u64", "s64", "long", "float", "double", "signed"}


class Site(collections.namedtuple("Site", "start end form ctype amp var off field")):
    """One raw access: TEXT[start:end] is replaced. ctype: the cast's pointee (None for u8 forms)."""


def find_sites(code, lo=0, hi=None):
    """Raw accesses in CODE[lo:hi] (comments already blanked), outermost form first, no overlaps."""
    hi = len(code) if hi is None else hi
    taken, out = [], []
    for form, pat in FORMS:
        for m in pat.finditer(code, lo, hi):
            a, b = m.span()
            if any(a < y and x < b for x, y in taken):
                continue
            g = m.groups()
            if form == "arrow":
                ctype, amp, var, off, field = g
            elif form in ("deref", "addr"):
                (ctype, amp, var, off), field = g, None
            else:
                (amp, var, off), ctype, field = g, "u8", None
            if form == "addr" and re.match(r"\s*(\[|->|\.|\+\+|--)", code[b:]):
                continue  # &V->m would take the postfix operator first
            if form == "bare":
                before = code[:a].rstrip()
                after = code[b:].lstrip()
                if not re.search(r"(?:[,=(]|\breturn)$", before) or not after[:1] in (")", ",", ";"):
                    continue
                if before.endswith("(") and re.search(r"(?:\)|\*)\s*\($", before):
                    continue  # (T *)((u8 *)V + K) or *((u8 *)V + K): their own forms
            ctype = re.sub(r"\s+", " ", ctype).replace(" *", "*").replace("*", " *").strip() if ctype else ctype
            taken.append((a, b))
            out.append(Site(a, b, form, ctype, bool(amp), var, int(off, 0), field))
    return sorted(out)


def declared_type(code, fstart, pos, var):
    """The aggregate type spelling V was declared with in the function before POS, as (spell, pointer)."""
    region = code[fstart:pos]
    pat = re.compile(r"(?:(?<=[(,;{}\n])|^)\s*(?:(?:const|volatile|register|static)\s+)*"
                     r"((?:struct|union)\s+\w+|[A-Za-z_]\w*)\b\s*((?:const\s+)?\*\s*(?:const\s+)?)?"
                     r"(?:\w+\s*(?:=[^,;(){}]*)?,\s*\*?\s*)*(?<!\w)" + re.escape(var) + r"\s*(?=[=;,)\[])")
    found = None
    for m in pat.finditer(region):
        spell = m.group(1)
        if spell in ("return", "else", "case", "goto", "sizeof") or spell in SCALARS:
            found = None
            continue
        found = (re.sub(r"\s+", " ", spell), bool(m.group(2)))
    return found


def global_type(pre, var):
    """(spell, pointer) of a file-scope declaration of VAR in preprocessed text, if any."""
    pat = re.compile(r"^\s*(?:extern\s+)?(?:(?:const|volatile)\s+)*((?:struct|union)\s+\w+|[A-Za-z_]\w*)\s*"
                     r"(\*\s*)?(?:(?:const|volatile)\s+)?" + re.escape(var) + r"\s*;", re.M)
    found = None
    for m in pat.finditer(pre):
        if m.group(1) not in SCALARS:
            found = (re.sub(r"\s+", " ", m.group(1)), bool(m.group(2)))
    return found


def layouts(rel, spells):
    """{spell: leaves} from layoutmerge's compiler probe of the unit's own declarations."""
    import layoutmerge
    text = layoutmerge.preprocess(rel)
    if text is None or not spells:
        return {}
    names = {f"__rf_{i}": s for i, s in enumerate(spells)}
    decl = "\n".join(f"extern {s} *{n};" for n, s in names.items())
    res = layoutmerge._scan_tu(rel, text + "\n" + decl + "\n", set(names))
    if "error" in res:
        if len(spells) == 1:
            return {}
        out = {}
        for s in spells:
            out.update(layouts(rel, [s]))
        return out
    out = {}
    for d in res["decls"]:
        st = res["structs"].get(d.get("struct"))
        if st and d["global"] in names:
            out[names[d["global"]]] = [lf for lf in st["leaves"] if lf.get("off") is not None]
    return out


def components(name):
    return re.findall(r"\[\d+\]|[A-Za-z_]\w*", name)


def join(parts):
    return "".join(p if p.startswith("[") else "." + p for p in parts).lstrip(".")


def candidates(leaves, off):
    """[(path, leaf or None)] of members starting at OFF, outermost first."""
    starts = collections.defaultdict(lambda: 1 << 30)
    for lf in leaves:
        parts = components(lf["name"])
        for k in range(1, len(parts)):
            starts[join(parts[:k])] = min(starts[join(parts[:k])], lf["off"])
    out, seen = [], set()
    for lf in leaves:
        parts = components(lf["name"])
        if lf["off"] == off:
            for k in range(1, len(parts)):
                p = join(parts[:k])
                if p not in seen and starts[p] == off and not parts[k].startswith("["):
                    seen.add(p)
                    out.append((p, None))
            if lf.get("bits") is None and lf["name"] not in seen:
                seen.add(lf["name"])
                out.append((lf["name"] + ("[0]" if lf.get("array") else ""), lf))
        elif lf.get("array") and lf.get("esize") and lf["off"] < off < lf["off"] + lf["size"] \
                and (off - lf["off"]) % lf["esize"] == 0 and lf["base"] in ("int", "ptr"):
            out.append((f"{lf['name']}[{(off - lf['off']) // lf['esize']}]", lf))
    out.sort(key=lambda c: len(components(c[0])))
    return out


def scalar_ok(ctype, lf):
    """Whether a member leaf has the integer cast's width and signedness."""
    w, sg = INTS[ctype]
    if lf is None or lf.get("bits") is not None or lf["base"] != "int":
        return False
    width = lf.get("esize") if lf.get("array") else lf.get("width")
    return width == w and (lf["signed"] == sg or w == 1 and ctype == "char")


def type_probe(rel, checks):
    """For each (ctype, spell, path): whether agbcc takes `ctype *x = &((spell *)0)->path;` silently."""
    import build
    import layoutmerge
    if not checks:
        return []
    text = layoutmerge.preprocess(rel)
    if text is None:
        return [False] * len(checks)
    body = layoutmerge.strip_bodies(text)
    lines = [f"{c} *__rfc_{i} = &(({s} *)0)->{p};" for i, (c, s, p) in enumerate(checks)]
    src = body + '\n# 1 "__rfprobe__"\n' + "\n".join(lines) + "\n"
    with tempfile.TemporaryDirectory() as td:
        ipath = os.path.join(td, "p.i")
        with open(ipath, "w") as f:
            f.write(src)
        r = subprocess.run([build.AGBCC, "-O2", "-mthumb-interwork", ipath, "-o", os.path.join(td, "p.s")],
                           capture_output=True, text=True, stdin=subprocess.DEVNULL)
    bad = {int(n) - 1 for n in re.findall(r"^__rfprobe__:(\d+):", r.stderr, re.M)}
    if r.returncode and not bad:
        return [False] * len(checks)
    return [i not in bad for i in range(len(checks))]


def spelled(site, path):
    """The member access that replaces SITE."""
    base = f"{site.var}.{path}" if site.amp else f"{site.var}->{path}"
    if site.form == "arrow":
        return f"{base}.{site.field}"
    if site.form in ("deref", "deref8"):
        return base
    if path.endswith("[0]"):
        return base[:-3]  # an array's address is its name
    return "&" + base


def plan(rel, text):
    """[(site, [replacement, ...]), ...] and report rows, for the text of unit REL."""
    import layoutmerge
    code = keepcheck.blank_comments(text)
    drafts = keepcheck.draft_ranges(text)
    pre = None
    sites = []
    for fstart, fend, _ in keepcheck.function_spans(text):
        if keepcheck.in_ranges(fstart, drafts):
            continue
        for s in find_sites(code, fstart, fend):
            line = code[code.rfind("\n", 0, s.start) + 1:s.start]
            if line.lstrip().startswith("#"):
                continue
            t = None if s.amp else declared_type(code, fstart, s.start, s.var)
            if t is None and not declared_type_scalar(code, fstart, s.start, s.var):
                if pre is None:
                    pre = layoutmerge.preprocess(rel) or ""
                t = global_type(pre, s.var)
            if t is None or t[1] == s.amp:
                continue  # a pointer needs no &, an object needs one
            sites.append((s, t[0]))
    spells = sorted({spell for _, spell in sites if is_type(spell, pre, rel)})
    lay = layouts(rel, spells)
    rows, cands, checks = [], [], []
    for s, spell in sites:
        leaves = lay.get(spell)
        if not leaves:
            continue
        mine = []  # (path, leaf, index of its type probe or None)
        for path, lf in candidates(leaves, s.off):
            if s.ctype in INTS and s.form != "bare":
                if scalar_ok(s.ctype, lf):
                    mine.append((path, lf, None))
            elif s.ctype in ("void", "u8"):  # an address passed on: any member, u8 ones first
                mine.append((path, lf, None))
            else:
                checks.append((s.ctype, spell, path))
                mine.append((path, lf, len(checks) - 1))
        cands.append((s, spell, mine))
    ok = type_probe(rel, checks)
    out = []
    for s, spell, mine in cands:
        good = [(p, lf) for p, lf, k in mine if k is None or ok[k]]
        line = text.count("\n", 0, s.start) + 1
        raw = " ".join(text[s.start:s.end].split())
        if not good:
            rows.append(f"{rel}:{line}: {raw} -- no {s.ctype} member of {spell} at {s.off:#x}")
            continue
        if s.form == "bare":
            good.sort(key=lambda c: not (c[1] and c[1]["base"] == "int" and c[1].get("esize", c[1].get("width")) == 1))
        out.append((s, [spelled(s, p) for p, _ in good], f"{rel}:{line}: {raw}"))
    return out, rows


def is_type(spell, pre, rel):
    """struct/union X, or a typedef name the unit declares."""
    if spell.startswith(("struct ", "union ")):
        return True
    if pre is None:
        import layoutmerge
        pre = layoutmerge.preprocess(rel) or ""
    return re.search(r"\btypedef\b[^;{]*\b%s\s*;|\}\s*%s\s*;" % (spell, spell), pre) is not None


def declared_type_scalar(code, fstart, pos, var):
    """Whether VAR is declared in the function as something other than an aggregate (a u8 *, an int)."""
    pat = re.compile(r"\b(?:" + "|".join(sorted(SCALARS)) + r")\s*\**\s*" + re.escape(var) + r"\s*[=;,)\[]")
    return pat.search(code[fstart:pos]) is not None


def process(rel, dry=False, log=print):
    unit = keepcheck.Unit(rel)
    items, rows = plan(rel, unit.text)
    applied = tried = 0
    if not items:
        return 0, 0, rows
    if dry:
        for s, reps, where in items:
            log(f"{where} -> {reps[0]}" + (f" (+{len(reps) - 1})" if len(reps) > 1 else ""))
        return 0, len(items), rows
    # everything at once first: most units take all or nothing
    new = unit.text
    for s, reps, _ in sorted(items, key=lambda it: -it[0].start):
        new = new[:s.start] + reps[0] + new[s.end:]
    tried = len(items)
    if unit.attempt(new):
        for s, reps, where in items:
            log(f"{where} -> {reps[0]}")
        applied = len(items)
    else:
        for s, reps, where in sorted(items, key=lambda it: -it[0].start):
            for rep in reps:
                if unit.attempt(unit.text[:s.start] + rep + unit.text[s.end:]):
                    log(f"{where} -> {rep}")
                    applied += 1
                    break
            else:
                rows.append(f"{where} -> {reps[0]} not applied ({unit.last})")
    if applied:
        unit.finish()
    return applied, tried, rows


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("paths", nargs="*", help="units or directories (default: src/)")
    ap.add_argument("--skip", default=os.path.join(ROOT, "build", "skip_files.txt"),
                    help="file of paths to leave alone (default: build/skip_files.txt, if present)")
    ap.add_argument("--dry-run", action="store_true", help="list the rewrites without editing")
    ap.add_argument("--report", help="append the review rows to this file")
    a = ap.parse_args(argv)
    applied = tried = 0
    rows = []
    for rel in keepcheck.src_files(a.paths, keepcheck.read_skip(a.skip)):
        if "(u8 *)" not in open(os.path.join(ROOT, rel)).read().replace("(const u8 *)", "(u8 *)"):
            continue
        n, t, r = process(rel, a.dry_run)
        applied, tried, rows = applied + n, tried + t, rows + r
    for r in rows:
        print(r)
    if a.report:
        with open(a.report, "a") as f:
            f.write("".join(r + "\n" for r in rows))
    print(f"rawfield: {applied} rewritten of {tried} tried; {len(rows)} rows for review")
    return 0


if __name__ == "__main__":
    sys.exit(main())
