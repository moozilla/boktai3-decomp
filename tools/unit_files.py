#!/usr/bin/env python3
"""List the C files covering a ROM range, in ROM order, and its unmatched functions.

  unit_files.py START END        (hex addresses, END exclusive)

Unmatched functions have no C definition; a merged unit keeps them as
INCLUDE_ASM (or a NONMATCHING draft) at their ROM position. Functions the
progress inventory does not know (gbadisasm gaps) are not listed here.
"""
import sys
from pathlib import Path

import progress
from romlib import ROOT
from split import c_functions


def main():
    lo, hi = (int(a, 16) for a in sys.argv[1:3])
    funcs, _ = progress.load_functions()
    addr = {n: a for n, a in funcs}
    addr.update({f"sub_{a:08X}": a for n, a in funcs})
    files, defined = [], set()
    for p in sorted(Path(ROOT, "src").rglob("*.c")):
        fs = [f for f in c_functions(p) if lo <= addr.get(f, -1) < hi]
        if fs:
            files.append((min(addr[f] for f in fs), p.relative_to(ROOT), fs))
            defined.update(addr[f] for f in c_functions(p, with_asm=False) if f in addr)
    for _, p, fs in sorted(files):
        print(p, " ".join(fs))
    missing = [f"{n} ({a:08X})" for n, a in funcs if lo <= a < hi and a not in defined]
    if missing:
        print("unmatched:", ", ".join(missing))


if __name__ == "__main__":
    main()
