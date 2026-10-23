#!/usr/bin/env python3
"""Align two Boktai ROMs: which byte ranges of A also exist in B, and where.

  romalign.py A.gba B.gba [--win 12] [--min 64] [--csv out.csv]

Windows of WIN halfwords (Thumb BL pairs masked) that occur exactly once in each
ROM are anchors; consecutive anchors with the same (B - A) delta merge into a
segment. Prints segments of at least MIN bytes and a summary of how much of A is
covered. Use it for language variants of one game (segments = shared data/code,
gaps = what was localized) and for different games (segments = shared engine).
"""
import argparse
import csv
import sys

import numpy as np

from crossgame import load, mask_bl, windows


def segments(a, b, win, gap):
    aw, bw = windows(a, win), windows(b, win)
    ua, ia, ca = np.unique(aw, return_index=True, return_counts=True)
    ub, ib, cb = np.unique(bw, return_index=True, return_counts=True)
    ua_u, ia_u = ua[ca == 1], ia[ca == 1]
    ub_u, ib_u = ub[cb == 1], ib[cb == 1]
    common, xa, xb = np.intersect1d(ua_u, ub_u, return_indices=True)
    pa = ia_u[xa].astype(np.int64)
    pb = ib_u[xb].astype(np.int64)
    o = np.argsort(pa)
    pa, pb = pa[o], pb[o]
    segs = []
    if len(pa) == 0:
        return segs
    delta = pb - pa
    s = 0
    for i in range(1, len(pa) + 1):
        if i == len(pa) or delta[i] != delta[s] or pa[i] - pa[i - 1] > gap:
            segs.append((int(pa[s]) * 2, (int(pa[i - 1]) + win) * 2, int(delta[s]) * 2))
            s = i
    return segs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a")
    ap.add_argument("b")
    ap.add_argument("--win", type=int, default=12)
    ap.add_argument("--min", type=int, default=64)
    ap.add_argument("--gap", type=int, default=24, help="max halfword gap between anchors in a segment")
    ap.add_argument("--csv")
    ap.add_argument("--quiet", action="store_true")
    o = ap.parse_args()
    a = mask_bl(load(o.a))
    b = mask_bl(load(o.b))
    segs = [s for s in segments(a, b, o.win, o.gap) if s[1] - s[0] >= o.min]
    cov = sum(e - s for s, e, _ in segs)
    used = len(a.tobytes()) if False else len(a) * 2
    # exclude trailing padding from the denominator
    raw = np.frombuffer(open(o.a, "rb").read(), dtype=np.uint8)
    nz = np.nonzero((raw != 0) & (raw != 0xFF))[0]
    used = int(nz[-1]) + 1 if len(nz) else len(raw)
    print(f"# {o.a} -> {o.b}: {cov} of {used} bytes ({100 * cov / used:.1f}%) in {len(segs)} segments >= {o.min} B", file=sys.stderr)
    out = csv.writer(open(o.csv, "w") if o.csv else sys.stdout)
    if not o.quiet:
        out.writerow(["a_start", "a_end", "len", "b_start", "delta"])
        for s, e, d in segs:
            out.writerow([f"{0x08000000 + s:08X}", f"{0x08000000 + e:08X}", e - s, f"{0x08000000 + s + d:08X}", d])


if __name__ == "__main__":
    main()
