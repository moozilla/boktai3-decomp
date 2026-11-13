#!/usr/bin/env python3
"""List the C files covering a ROM range, in ROM order, and its unmatched functions.

  unit_files.py START END [--paths-only]   (hex addresses, END exclusive)

Unmatched functions have no C definition; a merged unit keeps them as
INCLUDE_ASM (or a NONMATCHING draft) at their ROM position. Functions the
progress inventory does not know (gbadisasm gaps) are not listed here.

The normal listing warns when a file also defines functions outside the range.
--paths-only emits paths suitable for merge_unit.py, with no partial output if
a known function is unmatched or an existing unit crosses either boundary.
"""
import sys
import argparse
from pathlib import Path

import progress
from romlib import ROOT
from split import c_functions


def collect_units(root, funcs, lo, hi):
    """Include whole source files, and identify files crossing either boundary."""
    addr = {n: a for n, a in funcs}
    addr.update({f"sub_{a:08X}": a for n, a in funcs})
    files, defined, overlaps = [], set(), []
    for p in sorted(Path(root, "src").rglob("*.c")):
        all_functions = c_functions(p)
        fs = [f for f in all_functions if lo <= addr.get(f, -1) < hi]
        if fs:
            relative = p.relative_to(root)
            files.append((min(addr[f] for f in fs), relative, fs))
            defined.update(addr[f] for f in c_functions(p, with_asm=False) if f in addr)
            outside = [f for f in all_functions if f in addr and not lo <= addr[f] < hi]
            if outside:
                overlaps.append((relative, outside))
    missing = [f"{n} ({a:08X})" for n, a in funcs if lo <= a < hi and a not in defined]
    return sorted(files), missing, overlaps


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("start", type=lambda value: int(value, 16))
    parser.add_argument("end", type=lambda value: int(value, 16))
    parser.add_argument("--paths-only", action="store_true", help="print merge inputs only; fail on gaps or boundary crossings")
    args = parser.parse_args()
    if args.start >= args.end:
        parser.error("START must be less than END")
    funcs, _ = progress.load_functions()
    files, missing, overlaps = collect_units(ROOT, funcs, args.start, args.end)
    for path, outside in overlaps:
        print(f"outside range in {path}: {', '.join(outside)}", file=sys.stderr)
    if args.paths_only and (missing or overlaps):
        if missing:
            print("unmatched:", ", ".join(missing), file=sys.stderr)
        return 1
    for _, path, names in files:
        print(path if args.paths_only else f"{path} {' '.join(names)}")
    if missing:
        print("unmatched:", ", ".join(missing))
    return 0


if __name__ == "__main__":
    sys.exit(main())
