#!/usr/bin/env python3
"""Find and dump the text and font data of a Boktai 1/2/3 ROM.

  b12text.py font ROM [--sheet OUT.ppm]   font resource headers (narrow/wide glyph counts)
  b12text.py gcx ROM                      script image: string table, string data, language blocks
  b12text.py escapes ROM FUNC_ADDR        glyph escape table read by Text_DecodeGlyphEscape at FUNC_ADDR

The font resource (type `fnt` 0xA635) starts with `u16 narrow, u16 wide, u32 narrow_off,
u32 wide_off` where narrow_off is 0xC and wide_off is 0xC + 64 * narrow; narrow glyphs
are 8x16 4bpp (64 B), wide glyphs 16x16 4bpp (128 B, four 8x8 tiles). Boktai 2 and 3 only:
Boktai 1's text engine is different.

The script image (`gcx`, 0xA8D9) is {u32 timestamp, u32 procedure table ending in
0xFFFFFFFF, four u32 section offsets}; Text_LookupString indexes a table of u32 (bit 31
= text) at base+section[1], string data at base+section[2].
"""
import re
import struct
import sys
from pathlib import Path


def rom(path):
    return Path(path).read_bytes()


def find_fonts(d):
    out = []
    for m in re.finditer(rb"\x0c\x00\x00\x00", d):
        i = m.start()
        if i < 4 or i % 4:
            continue
        n, w = struct.unpack_from("<HH", d, i - 4)
        off = struct.unpack_from("<I", d, i + 4)[0]
        if 16 <= n <= 1024 and 16 <= w <= 4096 and off == 0xC + n * 64:
            out.append((i - 4, n, w))
    return out


def find_gcx(d):
    n = len(d) // 4
    w = struct.unpack_from("<%dI" % n, d)
    out = []
    for i in range(0x300000 // 4, n - 8):
        t = w[i]
        if not 0x3F000000 <= t < 0x44000000 or (w[i + 1] >> 24):
            continue
        j = i + 1
        while j < min(n, i + 40000) and w[j] != 0xFFFFFFFF:
            j += 1
        if j >= n - 5 or j - i < 1000:
            continue
        s = w[j + 1 : j + 5]
        if max(s) < 0x2000000 and sum((x & 0xFFFFFF) < 0x400000 for x in w[i + 1 : j]) > 0.98 * (j - i - 1):
            out.append((i * 4, j * 4 + 4, s))
    return out


def strings(d, base, s):
    tab, dat = base + s[1], base + s[2]
    n = (s[2] - s[1]) // 4
    ents = struct.unpack_from("<%dI" % n, d, tab)

    def get(i):
        e = ents[i]
        if not e >> 31:
            return None
        o = dat + (e & 0x7FFFFFFF)
        return d[o : o + 400].split(b"\0")[0]

    return n, get


def main():
    cmd, path = sys.argv[1], sys.argv[2]
    d = rom(path)
    if cmd == "font":
        for o, n, w in find_fonts(d):
            print(f"{0x8000000 + o:08X} narrow {n} wide {w}")
    elif cmd == "gcx":
        found = find_gcx(d)
        for o, base, s in found[:1]:
            n, get = strings(d, base, s)
            print(f"image {0x8000000 + o:08X}  procedure table ends {0x8000000 + base:08X}")
            print(f"body {0x8000000 + base + s[0]:08X}  string table {0x8000000 + base + s[1]:08X}  string data {0x8000000 + base + s[2]:08X}  strings {n}")
            # language blocks: the item-name list "EARTHLY NUT" restarts once per language in the EU ROM
            starts = [i for i in range(n) if (get(i) or b"").startswith(b"EARTHLY")]
            print("'EARTHLY NUT' at", starts)
    elif cmd == "escapes":
        a = int(sys.argv[3], 16) - 0x8000000
        ptr = [v for v in struct.unpack_from("<11I", d, a) if 0x08000000 <= v < 0x0A000000][-1]
        t = ptr - 0x8000000
        ents = []
        while True:
            k, v = struct.unpack_from("<HH", d, t)
            t += 4
            if k == 0:
                break
            ents.append((k, v))
        print(f"table {ptr:08X}, {len(ents)} entries (key after 0x1F -> glyph; 0x80xx = wide glyph xx)")
        print(" ".join(f"{k:02X}>{v:X}" for k, v in ents))


if __name__ == "__main__":
    main()
