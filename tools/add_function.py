#!/usr/bin/env python3
"""Register function starts in the inventory (symbols/progress_functions.csv).

  add_function.py ADDR...

The inventory is the one list of function starts: tools/disasm.py seeds gbadisasm with it,
split.py cuts any start gbadisasm still misses out of the raw bytes, and progress.py sizes every
function from it. Run `make` afterwards so gen/ and the build follow.
"""
import csv
import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
from romlib import ROOT  # noqa: E402
import progress  # noqa: E402


def add(addresses):
    with open(progress.INVENTORY, newline="") as f:
        have = [int(r["address"], 16) for r in csv.DictReader(f)]
    new = sorted(set(addresses) - set(have))
    progress.write_inventory(progress.INVENTORY, sorted(set(have) | set(new)))
    return new


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    try:
        new = add([int(a, 16) for a in sys.argv[1:]])
    except ValueError as e:
        sys.exit(f"add_function.py: {e}")
    print(f"{len(new)} new function start(s): " + (", ".join(f"{a:08X}" for a in new) or "none")
          + f" in {os.path.relpath(progress.INVENTORY, ROOT)}")


if __name__ == "__main__":
    main()
