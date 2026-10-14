#!/usr/bin/env python3
"""Decode the script string table with data/charmap.tsv.

  text.py dump [OUT]     every string (default build/text/strings.txt)
  text.py find WORD      strings containing WORD

Strings (Text_LookupString, 0821ADD4): u32 offsets at 08D6B1F8 (bit 31 marks
text) from base 08D74DDC, NUL-terminated. Bytes below 0x80 are ASCII (markup
such as <PROC=0>, newlines); a lead byte 0x80-0x85 and the next byte form a
glyph index (code - 0x8000) into the font, mapped by data/charmap.tsv.
Unknown glyphs print as {XXXX}. Output is ROM-derived: keep it under build/.
"""
import csv
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from romlib import ROOT, load_rom  # noqa: E402

TABLE, BASE, COUNT = 0x08D6B1F8, 0x08D74DDC, 9977


def charmap():
    m = {}
    with open(os.path.join(ROOT, "data", "charmap.tsv"), encoding="utf-8") as f:
        for ln in f:
            if ln[:1] in "#i" or not ln.strip():
                continue
            i, c, _ = ln.rstrip("\n").split("\t")
            m[int(i, 16)] = c
    return m


def decode(raw, cmap):
    out, i = [], 0
    while i < len(raw):
        b = raw[i]
        if b == 0:
            break
        if 0x80 <= b <= 0x85 and i + 1 < len(raw):
            g = (b - 0x80) << 8 | raw[i + 1]
            out.append(cmap.get(g, f"{{{g:04X}}}"))
            i += 2
        else:
            out.append(chr(b) if b >= 0x20 or b == 0x0A else f"\\x{b:02x}")
            i += 1
    return "".join(out)


def strings(rom):
    cmap = charmap()
    for n in range(COUNT):
        w, = struct.unpack_from("<I", rom, TABLE - 0x08000000 + n * 4)
        if not w & 0x80000000:
            continue
        o = BASE - 0x08000000 + (w & 0x7FFFFFFF)
        end = rom.index(b"\0", o)
        yield n, decode(rom[o:end], cmap)


def main():
    rom = load_rom()
    cmd = sys.argv[1] if len(sys.argv) > 1 else "dump"
    if cmd == "dump":
        out = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, "build", "text", "strings.txt")
        os.makedirs(os.path.dirname(out), exist_ok=True)
        with open(out, "w", encoding="utf-8") as f:
            for n, s in strings(rom):
                f.write(f"#{n}\n{s}\n")
        print(out)
    elif cmd == "find":
        for n, s in strings(rom):
            if sys.argv[2] in s:
                print(f"#{n}: {s[:200]!r}")


if __name__ == "__main__":
    main()
