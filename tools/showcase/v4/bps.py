#!/usr/bin/env python3
"""Minimal BPS patch writer and applier (the format Flips, Floating IPS and
most emulators' soft-patching read).

  python3 tools/showcase/v4/bps.py make SOURCE TARGET OUT.bps
  python3 tools/showcase/v4/bps.py apply SOURCE PATCH.bps OUT

Only SourceRead and TargetRead actions: unchanged runs are copied from the
source, changed bytes are stored. Good enough for a hack whose new data sits
in free space.
"""
import sys
import zlib


def _num(n):
    out = bytearray()
    while True:
        x = n & 0x7F
        n >>= 7
        if n == 0:
            out.append(0x80 | x)
            return out
        out.append(x)
        n -= 1


def make(src, dst):
    out = bytearray(b"BPS1") + _num(len(src)) + _num(len(dst)) + _num(0)
    i, n = 0, len(dst)
    while i < n:
        j = i
        while j < n and j < len(src) and src[j] == dst[j]:
            j += 1
        if j - i >= 4 or j == n:
            if j > i:
                out += _num((j - i - 1) << 2 | 0)
            i = j
            continue
        # changed bytes, until a run of 4+ unchanged ones
        j = i
        while j < n:
            if j < len(src) and src[j:j + 4] == dst[j:j + 4] and j + 4 <= min(n, len(src)):
                break
            j += 1
        out += _num((j - i - 1) << 2 | 1) + dst[i:j]
        i = j
    out += zlib.crc32(src).to_bytes(4, "little") + zlib.crc32(dst).to_bytes(4, "little")
    out += zlib.crc32(out).to_bytes(4, "little")
    return bytes(out)


def apply(src, patch):
    assert patch[:4] == b"BPS1"
    p = 4

    def num():
        nonlocal p
        d, s = 0, 1
        while True:
            x = patch[p]
            p += 1
            d += (x & 0x7F) * s
            if x & 0x80:
                return d
            s <<= 7
            d += s
    ss, ts, ms = num(), num(), num()
    assert ss == len(src) and zlib.crc32(src) == int.from_bytes(patch[-12:-8], "little"), "wrong source ROM"
    p += ms
    out = bytearray()
    sr = tr = 0
    end = len(patch) - 12
    while p < end:
        d = num()
        cmd, length = d & 3, (d >> 2) + 1
        if cmd == 0:
            out += src[len(out):len(out) + length]
        elif cmd == 1:
            out += patch[p:p + length]
            p += length
        else:
            off = num()
            off = (-1 if off & 1 else 1) * (off >> 1)
            if cmd == 2:
                sr += off
                out += src[sr:sr + length]
                sr += length
            else:
                tr += off
                for _ in range(length):
                    out.append(out[tr])
                    tr += 1
    assert len(out) == ts and zlib.crc32(out) == int.from_bytes(patch[-8:-4], "little"), "bad patch"
    return bytes(out)


if __name__ == "__main__":
    op, a, b, c = sys.argv[1:5]
    data = (make if op == "make" else apply)(open(a, "rb").read(), open(b, "rb").read())
    open(c, "wb").write(data)
    print(c, len(data))
