#!/usr/bin/env python3
"""Propose subsystem ranges for code no range covers yet, from runs of name prefixes.

  families.py [--min 3] [--gap 3]          print proposals (ROM order)
  families.py --write                      add them to symbols/progress_subsystems.csv

Function names follow Prefix_Action, so a contiguous run of one prefix in ROM
order is one object's code, as the existing ranges are. Outside the existing
ranges this walks the functions in order, extends a run over up to --gap
functions with another prefix or no name (the unnamed helpers of a family sit
between its named functions), and proposes runs of at least --min functions
with that prefix. Proposed ranges are marked "(auto, tentative)" with their
evidence, end on function boundaries, never overlap an existing range, and keep
the report's credit unchanged (grouping only changes how units are displayed).
Run tools/namer.py first: the more functions have names, the more this finds.
"""
import argparse
import csv
import os
import re
import sys
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import progress  # noqa: E402
from romlib import ROOT  # noqa: E402

CSV = Path(ROOT) / "symbols" / "progress_subsystems.csv"
LIBRARIES = Path(ROOT) / "symbols" / "progress_libraries.csv"


def prefix(name):
    if re.match(r"^sub_[0-9A-F]{8}$", name) or "_" not in name:
        return None
    return name.split("_", 1)[0]


def spaced(p):
    """ScenePanel -> Scene panel."""
    words = re.findall(r"[A-Z]+(?![a-z])|[A-Z][a-z0-9]*|[a-z0-9]+", p)
    return " ".join([words[0]] + [w if w.isupper() else w.lower() for w in words[1:]]) if words else p


def proposals(min_run, gap):
    funcs, _ = progress.load_functions()
    rows = list(csv.DictReader(CSV.open()))
    taken = [(int(r["start"], 16), int(r["end"], 16)) for r in rows + list(csv.DictReader(LIBRARIES.open()))]
    starts = [a for _, a in funcs] + [progress.CODE_END]
    free = [(n, a, starts[i + 1]) for i, (n, a) in enumerate(funcs)
            if not any(lo <= a < hi for lo, hi in taken)]
    out, i = [], 0
    while i < len(free):
        p = prefix(free[i][0])
        if not p:
            i += 1
            continue
        j, last, misses = i, i, 0
        while j + 1 < len(free) and free[j + 1][1] == free[j][2]:    # stay contiguous
            j += 1
            if prefix(free[j][0]) == p:
                last, misses = j, 0
            else:
                misses += 1
                if misses > gap:
                    break
        run = free[i:last + 1]
        hits = sum(1 for n, _, _ in run if prefix(n) == p)
        if hits >= min_run:
            out.append((p, run[0][1], run[-1][2], len(run), hits))
            i = last + 1
        else:
            i += 1
    return out, rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min", type=int, default=3)
    ap.add_argument("--gap", type=int, default=3)
    ap.add_argument("--write", action="store_true")
    a = ap.parse_args()
    found, rows = proposals(a.min, a.gap)
    total = sum(hi - lo for _, lo, hi, _, _ in found)
    for p, lo, hi, n, hits in found:
        print(f"{lo:08X}-{hi:08X}  {spaced(p):40s} {n:4d} functions, {hits} {p}_*")
    print(f"{len(found)} ranges, {total} bytes ({total / (progress.CODE_END - 0x08000000):.1%} of code)")
    if a.write:
        names = {r["name"] for r in rows}
        for p, lo, hi, n, hits in found:
            name = f"{spaced(p)} (auto, tentative)"
            k = 2
            while name in names:
                name = f"{spaced(p)} {k} (auto, tentative)"
                k += 1
            names.add(name)
            rows.append({"name": name, "start": f"{lo:08X}", "end": f"{hi:08X}",
                         "name_files": "",
                         "evidence": f"tools/families.py: {hits} of {n} contiguous functions named {p}_*"})
        rows.sort(key=lambda r: int(r["start"], 16))
        with CSV.open("w", newline="") as fh:
            w = csv.DictWriter(fh, fieldnames=["name", "start", "end", "name_files", "evidence"],
                               lineterminator="\n")
            w.writeheader()
            w.writerows(rows)
        print(f"wrote {CSV.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
