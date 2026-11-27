#!/usr/bin/env python3
"""Write a single-use literal local in place, keeping each fold on MATCH.

  fold_literals.py [PATH ...] [--skip build/skip_files.txt] [--dry-run] [--report FILE]

tools/concessions.py warns about `literal-local` and `const-local`: a local that only parks a
constant (`u32 mask = 0x40;` ... `if (flags & mask)`), often left over from steering the
compiler. When the local is read exactly once, this writes the literal at that read and drops the
declaration (`if (flags & 0x40)`), and keeps the change only when tools/check.py still reports
MATCH with no new compiler warning (tools/keepcheck.py). A local read more than once keeps its
name (a named constant used twice is ordinary C, and the owner rules on those) unless
`--every-read` is given: then the literal is written at every read, as long as no read is a
write (`x++`, `x = ...`), and the same MATCH rule decides.

Two shapes are folded, each with its declaration alone on its line:

    u32 mask = 0x40;                 (the declaration holds the literal)
    u32 mask;  ...  mask = 0x40;     (a bare declaration, then one literal assignment)

The value must fit the local's type (`u8 x = 0x1FF` would change the value), except a negative
or `~` literal of an unsigned word whose read is an operand of `&`, `|` or `^`, where the
word's bits are the same. Not folded: a read under `sizeof`, a negative literal after `+` or `-`
(that is `subtract-negative`'s form), a local declared twice in the function, anything inside
`#ifdef NONMATCHING` or a macro, and a fold that gives the linter a new finding (`off = 0x8D0`
read as `(u8 *)p + off` would become a raw-offset error). Locals the linter excuses with
`concession-ok` are left alone.
"""
from __future__ import annotations

import argparse
import os
import re
import sys
from dataclasses import dataclass

TOOLS = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, TOOLS)
import concessions  # noqa: E402
import keepcheck  # noqa: E402
from keepcheck import ROOT  # noqa: E402

LIT = r"(?:-\s*|~\s*)?(?:0[xX][0-9A-Fa-f]+|\d+)[uUlL]*"
TYPES = {"u8": (0, 0xFF), "s8": (-0x80, 0x7F), "u16": (0, 0xFFFF), "s16": (-0x8000, 0x7FFF),
         "u32": (0, 0xFFFFFFFF), "s32": (-0x80000000, 0x7FFFFFFF), "int": (-0x80000000, 0x7FFFFFFF),
         "unsigned": (0, 0xFFFFFFFF), "bool8": (0, 0xFF), "bool16": (0, 0xFFFF), "bool32": (0, 0xFFFFFFFF),
         "short": (-0x8000, 0x7FFF), "char": (0, 0xFF), "long": (-0x80000000, 0x7FFFFFFF)}
DECL = re.compile(r"^([ \t]*)((?:(?:register|const)\s+)*(?:unsigned\s+|signed\s+)?(\w+))\s+(\w+)\s*(?:=\s*(" + LIT
                  + r"))?\s*;[ \t]*(?://[^\n]*)?\n", re.M)
ASSIGN = r"^[ \t]*{0}\s*=\s*(" + LIT + r")\s*;[ \t]*\n"


def value(lit):
    s = re.sub(r"\s+", "", lit).rstrip("uUlL")
    op = s[0] if s.startswith(("~", "-")) else ""
    digits = s[len(op):]
    v = int(digits, 16 if digits.lower().startswith("0x") else 8 if digits.startswith("0") else 10)
    return ~v if op == "~" else -v if op == "-" else v


def type_range(decl_type):
    words = decl_type.split()
    base = words[-1]
    if base in ("unsigned", "signed") or (len(words) > 1 and words[-2] == "unsigned" and base in ("int", "char", "short", "long")):
        return (0, {"char": 0xFF, "short": 0xFFFF}.get(base, 0xFFFFFFFF)) if "unsigned" in words else TYPES["int"]
    return TYPES.get(base)


def reads(code, name, lo, hi):
    """Offsets of NAME as an identifier in CODE[lo:hi], member accesses (p->name, s.name) excluded."""
    pat = re.compile(r"(?<![\w.])\b%s\b" % re.escape(name))
    return [m.start() for m in pat.finditer(code, lo, hi)
            if not re.search(r"(?:->|\.)\s*$", code[max(0, m.start() - 4):m.start()])]


def address_or_size(before):
    """Whether the read follows `sizeof` or a unary `&` (BEFORE: the code up to it, stripped)."""
    if re.search(r"\bsizeof\s*\(?$", before):
        return True
    return re.search(r"(?:^|[(,=?:;{}]|\breturn)\s*&$", before) is not None


@dataclass(frozen=True)
class Fold:
    name: str
    literal: str
    uses: tuple
    edits: tuple
    description: str


def edit_text(text, edits):
    for start, end, replacement in sorted(edits, reverse=True):
        text = text[:start] + replacement + text[end:]
    return text


def folds(text, rel=None, every=False, *, sites=None):
    """Map linter offsets to individual folds, or all reads of a binding with EVERY."""
    code = keepcheck.blank_comments(text)
    drafts = keepcheck.draft_ranges(text)
    if sites is None:
        sites = concessions.sites(text, rel, kinds=("literal-local", "const-local"), split_locals=True)
    warned = {s.offset for s in sites if not s.excused}
    out = {}
    for fstart, fend, fname in keepcheck.function_spans(text):
        if keepcheck.in_ranges(fstart, drafts):
            continue
        for d in DECL.finditer(code, fstart, fend):
            indent, dtype, base, name, lit = d.groups()
            if base in ("return", "goto", "case", "else") or dtype.split()[-1] == name:
                continue
            line = text.count("\n", 0, d.start()) + 1
            offsets = {d.start(4)}
            body_end = concessions._scope_end(code, d.end())
            cut = [(d.start(), d.end())]
            if lit is None:  # u32 mask; ... mask = 0x40;
                first = reads(code, name, d.end(), body_end)
                if not first:
                    continue
                a = re.compile(ASSIGN.format(re.escape(name)), re.M).match(code, code.rfind("\n", 0, first[0]) + 1)
                if not a or a.start(1) < first[0]:
                    continue
                line = text.count("\n", 0, a.start()) + 1
                offsets.add(first[0])
                lit = a.group(1)
                cut.append((a.start(), a.end()))
                lo = a.end()
            else:
                lo = d.end()
            if not offsets & warned:  # const-local warns at the declaration
                continue
            uses = reads(code, name, lo, body_end)
            if not uses or (len(uses) != 1 and not every) \
                    or len(reads(code, name, d.end(), body_end)) != len(uses) + (len(cut) - 1):
                continue
            if re.search(r"\b(?:\w+\s+)+\**\s*%s\s*[=;,\[)]" % re.escape(name), code[fstart:fend].replace(
                    code[d.start():d.end()], "", 1)):
                continue  # declared again (shadowed) or a parameter of the same name
            try:
                v = value(lit)
            except ValueError:
                continue
            rng = type_range(dtype)
            if rng is None:
                continue
            negative = lit.lstrip().startswith(("-", "~"))
            ok = True
            for use in uses:
                before = code[:use].rstrip()
                after = code[use + len(name):].lstrip()
                if re.match(r"(?:[-+*/%&|^]|<<|>>)?=(?!=)|\+\+|--", after) or re.search(r"(?:\+\+|--)$", before):
                    ok = False  # written again: not a parked constant
                    break
                if not rng[0] <= v <= rng[1]:
                    bitwise = re.search(r"(?:[&|^]=?)$", before) and not before.endswith(("&&", "||")) \
                        or re.match(r"[&|^](?![&|])", after)
                    if not (negative and rng == (0, 0xFFFFFFFF) and bitwise):
                        ok = False
                        break
                if address_or_size(before):
                    ok = False
                    break
                if negative and lit.lstrip().startswith("-") and before[-1:] in ("-", "+"):
                    ok = False
                    break
            if not ok:
                continue
            edits = [(a, b, "") for a, b in cut]
            edits.extend((use, use + len(name), re.sub(r"\s+", "", lit)) for use in uses)
            where = "its one use" if len(uses) == 1 else f"its {len(uses)} uses"
            fold = Fold(name, lit, tuple(uses), tuple(sorted(edits)),
                        f"{rel}:{line}: {name} = {lit} folded into {where} in {fname}")
            for offset in sorted(offsets & warned):
                out[offset] = fold
    return out


def candidates(text, rel=None, every=False):
    """[(new text, description)] for the folds selected by this tool."""
    return [(edit_text(text, fold.edits), fold.description) for fold in folds(text, rel, every).values()]


def adds_concession(old, new, rel):
    """Whether NEW has more unexcused linter findings of some kind than OLD (folding `off = 0x8D0`
    into `(u8 *)p + off` makes a raw-offset error)."""
    import collections

    def count(text):
        return collections.Counter(s.kind for s in concessions.sites(text, rel) if not s.excused)
    return bool(count(new) - count(old))


def process(rel, dry=False, log=print, every=False):
    unit = keepcheck.Unit(rel)
    applied = tried = 0
    rows = []
    failed = set()
    while True:
        progress = False
        for new, what in candidates(unit.text, rel, every):
            if what in failed:
                continue
            tried += 1
            if dry:
                log(what)
                failed.add(what)
                continue
            if adds_concession(unit.text, new, rel):
                failed.add(what)
                rows.append(f"{what}: not applied (it would add a linter finding)")
                continue
            if unit.attempt(new):
                applied += 1
                log(what)
                progress = True
                break  # offsets moved: plan again
            failed.add(what)
            rows.append(f"{what}: not applied ({unit.last})")
        if not progress:
            break
    if applied:
        unit.finish()
    return applied, tried, rows


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("paths", nargs="*", help="units or directories (default: src/)")
    ap.add_argument("--skip", default=os.path.join(ROOT, "build", "skip_files.txt"),
                    help="file of paths to leave alone (default: build/skip_files.txt, if present)")
    ap.add_argument("--dry-run", action="store_true", help="list the folds without editing")
    ap.add_argument("--report", help="append the review rows to this file")
    ap.add_argument("--every-read", action="store_true",
                    help="also fold a local read more than once, writing the literal at every read")
    a = ap.parse_args(argv)
    applied = tried = 0
    rows = []
    for rel in keepcheck.src_files(a.paths, keepcheck.read_skip(a.skip)):
        n, t, r = process(rel, a.dry_run, every=a.every_read)
        applied, tried, rows = applied + n, tried + t, rows + r
    for r in rows:
        print(r)
    if a.report:
        with open(a.report, "a") as f:
            f.write("".join(r + "\n" for r in rows))
    print(f"fold_literals: {applied} folded of {tried} tried; {len(rows)} rows for review")
    return 0


if __name__ == "__main__":
    sys.exit(main())
