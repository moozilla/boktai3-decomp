#!/usr/bin/env python3
"""Find the Boktai 3 functions that exist in another Boktai ROM.

  crossgame.py OTHER.gba [--out matches.csv] [--win 8]

Every Thumb function of symbols/functions.csv is cut into overlapping windows of
WIN halfwords (BL pairs masked, so call targets do not matter). Windows are looked
up in an index of the other ROM; the (target - source) offset that most windows
agree on is the function's address there. Output columns:

  b3addr,name,size,other_addr,coverage,exact

coverage = fraction of the function's windows found at that one offset (1.0 means
every window matched, which with exact=1 means identical but for BL targets and
literal-pool words). Windows that occur more than MAXHITS times are ignored.
"""
import argparse
import csv
import sys
from collections import Counter
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parent.parent
CODE_END = 0x24DAFA
MAXHITS = 6


def load(path, limit=None):
    d = Path(path).read_bytes()
    if limit:
        d = d[:limit]
    return np.frombuffer(d[: len(d) & ~1], dtype="<u2").astype(np.uint64)


def mask_bl(h):
    """Replace both halves of every BL (F000-F7FF, F800-FFFF) with constants."""
    h = h.copy()
    hi = (h >= 0xF000) & (h < 0xF800)
    lo = (h[1:] >= 0xF800)
    first = hi[:-1] & lo
    idx = np.nonzero(first)[0]
    h[idx] = 0xF000
    h[idx + 1] = 0xF800
    # literal-pool words loaded by LDR Rd,[PC,#imm] hold addresses that move between ROMs
    ld = np.nonzero((h >> np.uint64(11)) == 9)[0]
    tgt = ((ld + 2) & ~1) + (h[ld] & np.uint64(0xFF)).astype(np.int64) * 2
    tgt = tgt[tgt + 1 < len(h)]
    h[tgt] = 0xAAAA
    h[tgt + 1] = 0xBBBB
    return h


def shape(h):
    """Opcode-class only: drop registers and immediates (survives struct layout changes)."""
    h = h.copy()
    alu = (h >= 0x4000) & (h < 0x4400)
    cls = h >> np.uint64(11)
    cls[alu] = h[alu] >> np.uint64(6)
    cls[(h >= 0xF000)] = 0xF000 >> 11
    return cls


def windows(h, win):
    n = len(h) - win + 1
    acc = np.zeros(n, dtype=np.uint64)
    for k in range(win):
        acc = acc * np.uint64(0x100000001B3) + h[k : k + n] + np.uint64(1)
        acc ^= acc >> np.uint64(29)
    return acc


def functions():
    rows = []
    with open(ROOT / "symbols/functions.csv") as f:
        for r in csv.DictReader(f):
            rows.append((int(r["addr"], 16), r["name"], r["mode"]))
    rows.sort()
    out = []
    for i, (a, n, m) in enumerate(rows):
        if m != "thumb" or a >= 0x08000000 + CODE_END:
            continue
        end = rows[i + 1][0] if i + 1 < len(rows) else 0x08000000 + CODE_END
        out.append((a, n, end - a))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("other")
    ap.add_argument("--out")
    ap.add_argument("--win", type=int, default=8)
    ap.add_argument("--shape", action="store_true", help="fuzzy: match opcode classes only")
    ap.add_argument("--base", default=str(ROOT / "baserom.gba"))
    a = ap.parse_args()
    win = a.win

    base = mask_bl(load(a.base))
    other = mask_bl(load(a.other))
    if a.shape:
        base, other = shape(base), shape(other)
    bw = windows(base, win)
    ow = windows(other, win)
    order = np.argsort(ow, kind="stable")
    sorted_ow = ow[order]

    res = []
    for addr, name, size in functions():
        off = addr - 0x08000000
        if size < 2 * win + 2:
            continue
        o0 = off // 2
        nwin = size // 2 - win + 1
        votes = Counter()
        total = 0
        for i in range(nwin):
            hsh = bw[o0 + i]
            lo = np.searchsorted(sorted_ow, hsh, "left")
            hi = np.searchsorted(sorted_ow, hsh, "right")
            if hi == lo or hi - lo > MAXHITS:
                continue
            for p in order[lo:hi]:
                votes[int(p) - i] += 1
        if not votes:
            res.append((addr, name, size, 0, 0.0, 0))
            continue
        pos, cnt = votes.most_common(1)[0]
        cov = cnt / nwin
        oaddr = 0x08000000 + pos * 2
        seg_b = base[o0 : o0 + size // 2]
        seg_o = other[pos : pos + size // 2]
        exact = int(len(seg_o) == len(seg_b) and bool((seg_b == seg_o).all()))
        res.append((addr, name, size, oaddr, cov, exact))

    w = csv.writer(open(a.out, "w") if a.out else sys.stdout)
    w.writerow(["b3addr", "name", "size", "other_addr", "coverage", "exact"])
    for addr, name, size, oaddr, cov, exact in res:
        w.writerow([f"{addr:08X}", name, size, f"{oaddr:08X}" if oaddr else "", f"{cov:.2f}", exact])


if __name__ == "__main__":
    main()
