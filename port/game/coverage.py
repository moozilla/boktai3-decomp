#!/usr/bin/env python3
"""Compare function coverage of the port (--trace build) with the mGBA harness.

  python3 port/game/coverage.py MGBA_DIR/total.exec PORT.exec

Lists ROM functions mGBA entered that the port did not, and the reverse, in
ROM order. Assembly-only routines (symbols/progress_asm.csv) are skipped.
"""
import csv
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def main():
    mgba, port = (Path(p).read_bytes() for p in sys.argv[1:3])
    skip = []
    for r in csv.DictReader(open(ROOT / 'symbols/progress_asm.csv')):
        skip.append((int(r['start'], 16), int(r['end'], 16)))
    only_m, only_p = [], []
    for r in csv.DictReader(open(ROOT / 'symbols/functions.csv')):
        a = int(r['addr'], 16)
        if not 0x08000000 <= a < 0x09000000 or any(lo <= a < hi for lo, hi in skip):
            continue
        i = (a - 0x08000000) >> 1
        if mgba[i] and not port[i]:
            only_m.append(r)
        elif port[i] and not mgba[i]:
            only_p.append(r)
    print(f'entered only on mGBA: {len(only_m)}; only in the port: {len(only_p)}')
    for r in only_m:
        print('  mGBA', r['addr'], r['name'])
    for r in only_p:
        print('  port', r['addr'], r['name'])


if __name__ == '__main__':
    main()
