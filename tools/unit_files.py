#!/usr/bin/env python3
"""List the C files covering a ROM range, in ROM order, and its unmatched functions.

  unit_files.py START END [--strict]       (hex addresses, END exclusive)

A merged file can extend beyond the requested range. Such files are reported on
stderr; --strict rejects the listing so a merge script cannot silently absorb a
neighboring subsystem. This command never changes source files.

Unmatched functions have no C definition; a merged unit keeps them as
INCLUDE_ASM (or a NONMATCHING draft) at their ROM position. Functions the
progress inventory does not know (gbadisasm gaps) are not listed here.
"""
import argparse
import sys
from dataclasses import dataclass
from pathlib import Path

import progress
from romlib import ROOT
from split import c_functions


@dataclass(frozen=True)
class Unit:
    start: int
    path: Path
    functions: tuple[str, ...]
    outside: tuple[tuple[str, int], ...]


def collect_units(root, lo, hi, funcs):
    """Return units intersecting [LO, HI), including any functions outside it."""
    if lo >= hi:
        raise ValueError("START must be less than END")
    root = Path(root)
    addr = {n: a for n, a in funcs}
    addr.update({f"sub_{a:08X}": a for n, a in funcs})
    units, defined = [], set()
    for path in sorted((root / "src").rglob("*.c")):
        names = c_functions(path)
        selected = tuple(n for n in names if lo <= addr.get(n, -1) < hi)
        if not selected:
            continue
        outside = tuple((n, addr[n]) for n in names if n in addr and not lo <= addr[n] < hi)
        units.append(Unit(min(addr[n] for n in selected), path.relative_to(root), selected, outside))
        defined.update(addr[n] for n in c_functions(path, with_asm=False) if n in addr)
    units.sort(key=lambda unit: (unit.start, unit.path))
    missing = [(n, a) for n, a in funcs if lo <= a < hi and a not in defined]
    return units, missing


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("start", type=lambda value: int(value, 16))
    parser.add_argument("end", type=lambda value: int(value, 16))
    parser.add_argument("--strict", action="store_true", help="reject files spanning either range boundary")
    args = parser.parse_args(argv)
    if args.start >= args.end:
        parser.error("START must be less than END")
    funcs, _ = progress.load_functions()
    units, missing = collect_units(ROOT, args.start, args.end, funcs)
    crossing = [unit for unit in units if unit.outside]
    for unit in crossing:
        names = ", ".join(f"{name} ({address:08X})" for name, address in unit.outside)
        print(f"outside range: {unit.path}: {names}", file=sys.stderr)
    if args.strict and crossing:
        # Do not emit a partial list that a caller might mistakenly merge.
        return 1
    for unit in units:
        print(unit.path, " ".join(unit.functions))
    if missing:
        print("unmatched:", ", ".join(f"{name} ({address:08X})" for name, address in missing))
    return 0


if __name__ == "__main__":
    sys.exit(main())
