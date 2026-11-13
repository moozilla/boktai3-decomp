#!/usr/bin/env python3
"""List the C files covering a ROM range, in ROM order, and its unmatched functions.

  unit_files.py START END [--paths-only]  (hex addresses, END exclusive)

Unmatched functions have no C definition; a merged unit keeps them as
INCLUDE_ASM (or a NONMATCHING draft) at their ROM position. Functions the
progress inventory does not know (gbadisasm gaps) are not listed here.
Reviewed assembly ranges from symbols/progress_asm.csv are labeled separately.
--paths-only prints one path per line, with gap diagnostics on stderr, for merge
input lists or commit manifests.
"""
import argparse
import sys
from pathlib import Path

import progress
from romlib import ROOT
from split import c_functions


def covering_files(root, lo, hi, funcs):
    """ROM-ordered files and inventory functions without a C definition."""
    root = Path(root)
    addr = {n: a for n, a in funcs}
    addr.update({f"sub_{a:08X}": a for n, a in funcs})
    files, defined = [], set()
    for p in sorted((root / "src").rglob("*.c")):
        fs = [f for f in c_functions(p) if lo <= addr.get(f, -1) < hi]
        if fs:
            files.append((min(addr[f] for f in fs), p.relative_to(root), fs))
            defined.update(addr[f] for f in c_functions(p, with_asm=False) if f in addr)
    missing = [(n, a) for n, a in funcs if lo <= a < hi and a not in defined]
    return sorted(files), missing


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("start", type=lambda value: int(value, 16))
    parser.add_argument("end", type=lambda value: int(value, 16))
    parser.add_argument("--paths-only", action="store_true",
                        help="print only file paths; send gap diagnostics to stderr")
    args = parser.parse_args(argv)
    if args.start >= args.end:
        parser.error("START must be less than the exclusive END")
    funcs, _ = progress.load_functions()
    files, missing = covering_files(ROOT, args.start, args.end, funcs)
    for _, path, names in files:
        print(path if args.paths_only else f"{path} {' '.join(names)}")
    protected = progress.intentional_asm(missing, set(), progress.load_asm_ranges())
    diagnostics = sys.stderr if args.paths_only else sys.stdout
    for intentional, label in ((False, "unmatched"),
                               (True, "protected assembly (symbols/progress_asm.csv)")):
        entries = [f"{name} ({address:08X})" for name, address in missing
                   if (name in protected) == intentional]
        if entries:
            print(f"{label}:", ", ".join(entries), file=diagnostics)


if __name__ == "__main__":
    main()
