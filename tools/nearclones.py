#!/usr/bin/env python3
"""List unmatched functions that are near copies of a matched one.

  nearclones.py [--min-ratio 0.68] [--max-len 260] [--window 0.35] [--all]

Exact clones are clones.py's job. A near clone is the same code over another
struct layout, with other constants, an extra call or a missing branch, so its
normalized instructions (clones.parse(), registers abstracted) are similar but
not identical. Candidates are matched functions within --window of the length
whose instruction-trigram overlap is at least 0.5; they are ranked by
difflib's ratio. Interior labels (disassembler starts the inventory does not list) are skipped.

Each row shows where the unmatched function lives: "asm" (no C yet), "unit
FILE" (an INCLUDE_ASM in a src/*.c unit), or "draft N%" (a NONMATCHING draft
and its recorded fuzzy score). Rows are sorted by the gap between the best
sibling's ratio and the draft's score: an old draft written without the
sibling often scores well below what porting the sibling's C reaches, so the
largest gaps are the cheapest wins. Port by copying the sibling's C (its exact
spelling, structs with the new offsets) and fixing what check.py reports;
--all also lists functions without a usable gap.
"""
import argparse
import collections
import difflib
import glob
import os
import re

import clones
from romlib import ROOT
from split import function_nonentries
from worklist import done_names

REG = re.compile(r"\b(r\d+|sb|sl|fp|ip)\b")
INCLUDE = re.compile(r'INCLUDE_ASM\("[^"]*",\s*(\w+)\)')
FUZZY = re.compile(r"fuzzy:\s*([\d.]+)%")


def locations():
    """function name -> "draft N%" / "unit FILE" for every INCLUDE_ASM placeholder"""
    where = {}
    for path in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        text = open(path).read()
        rel = os.path.relpath(path, ROOT)
        for m in INCLUDE.finditer(text):
            name = m.group(1)
            block = text.rfind("#ifdef NONMATCHING", 0, m.start())
            if block >= 0 and text.rfind("#endif", block, m.start()) >= 0:
                block = -1
            fuzzy = FUZZY.search(text, block, m.start()) if block >= 0 else None
            if fuzzy:
                where[name] = f"draft {float(fuzzy.group(1)):.1f}%"
            elif rel.startswith(os.path.join("src", "fn")):
                where[name] = "draft ?"
            else:
                where[name] = f"unit {rel}"
    return where


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--min-ratio", type=float, default=0.68)
    ap.add_argument("--max-len", type=int, default=260)
    ap.add_argument("--window", type=float, default=0.35)
    ap.add_argument("--all", action="store_true")
    args = ap.parse_args()

    clones.LOOSE = True
    funcs = {n: (v[0], [REG.sub("R", l) for l in v[1]]) for n, v in clones.parse().items()}
    done = done_names()
    nonentries = function_nonentries()

    def usable(n):
        lines = funcs[n][1]
        return len(lines) >= 12 and "DATA" not in lines

    unmatched = [n for n in funcs if n not in done and usable(n) and len(funcs[n][1]) <= args.max_len
                 and funcs[n][0] not in nonentries]
    matched = [n for n in funcs if n in done and usable(n)]
    grams = {n: collections.Counter(zip(funcs[n][1], funcs[n][1][1:], funcs[n][1][2:]))
             for n in unmatched + matched}
    where = locations()

    rows = []
    for u in unmatched:
        lu = len(funcs[u][1])
        gu = grams[u]
        best = []
        for m in matched:
            lm = len(funcs[m][1])
            if abs(lm - lu) > max(4, lu * args.window):
                continue
            gm = grams[m]
            overlap = 2 * sum((gu & gm).values()) / (sum(gu.values()) + sum(gm.values()) or 1)
            if overlap < 0.5:
                continue
            ratio = difflib.SequenceMatcher(None, funcs[u][1], funcs[m][1], autojunk=False).ratio()
            if ratio >= args.min_ratio:
                best.append((ratio, m))
        if not best:
            continue
        best.sort(reverse=True)
        status = where.get(u, "asm")
        score = float(status.split()[1].rstrip("%")) / 100 if status.startswith("draft") and "?" not in status else 0.0
        gap = best[0][0] - score
        if gap <= 0 and not args.all:
            continue
        rows.append((gap, u, lu, status, best[:3]))
    rows.sort(key=lambda r: -r[0])
    for gap, u, lu, status, best in rows:
        sibs = ", ".join(f"{m}({r:.3f})" for r, m in best)
        print(f"{gap:+.3f} {u} 0x{funcs[u][0]:08X} {lu} [{status}] <- {sibs}")


if __name__ == "__main__":
    main()
