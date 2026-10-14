#!/usr/bin/env python3
"""The ROM's resource archives: list them and extract their entries.

  archive.py list                 directory and per-archive summary
  archive.py entries TYPE         one archive's entries (hash, offset, size, magic)
  archive.py extract [TYPE...]    write build/assets/archives/TYPE/HASH.bin

Resources are found by ScriptProc_Start(type, name) (0821A520): the type hash
selects one of the directory records at 08614D6C (u32 key, archive pointer;
zero-terminated, binary-searched by 0821A4E8), and the name hash is searched in
that archive. Hashes are the 16-bit string hash of sub_08219BD8.

Archive layout (offsets relative to the archive start):
  +0  u32 count   +4 u32 hash table   +8 u32 offset table   +C u32 data start
  hash table: count sorted u16 name hashes; offset table: count u32 offsets.
An entry ends where the next one starts; the last ends at the archive's size,
taken here as the next archive or ROM object (an upper bound, see `size`).
Extracted files are ROM-derived and stay under build/.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from romlib import ROOT, load_rom  # noqa: E402

ROM_BASE = 0x08000000
DIRECTORY = 0x08614D6C

# type hash (ScriptProc_Start's first argument) -> directory key, from 0821A520
TYPE_KEYS = {
    0x922E: 0x92255130, 0x92B3: 0x9305D710, 0x9B1B: 0x9A654679, 0x98F5: 0x9B052117,
    0xA635: 0xA7056D24, 0xA8D9: 0xA8D949BC, 0xAE6C: 0xAF05AC2C, 0xC091: 0xC305E53E,
    0xCB05: 0xC8E55F29, 0xCEEF: 0xCEE54F2D, 0xCEAA: 0xCF050A4D,
}
# extension strings whose hash equals the type hash (see docs/DATA.md)
TYPE_NAMES = {0x922E: "anm", 0xA635: "fnt", 0xA8D9: "gcx", 0xC091: "map", 0xCEEF: "ptn"}


def strhash(s):
    h = 0
    for c in s.encode() if isinstance(s, str) else s:
        h = ((h << 5) | (h >> 11)) & 0xFFFF
        h = (h + c) & 0xFFFF
    return h


def directory(rom):
    out, o = [], DIRECTORY - ROM_BASE
    while True:
        key, ptr = struct.unpack_from("<II", rom, o)
        if key == 0:
            return out
        out.append((key, ptr))
        o += 8


def type_label(key):
    for t, k in TYPE_KEYS.items():
        if k == key:
            return TYPE_NAMES.get(t, f"t{t:04X}"), t
    return f"k{key:08X}", None


def archives(rom):
    """[(label, type hash, base address, entries)] with entries [(hash, addr, size)]."""
    d = directory(rom)
    bases = sorted(p for _, p in d)
    out = []
    for key, base in d:
        if key == TYPE_KEYS[0xA8D9]:
            continue  # gcx: one compiled-script image, not an archive (scripts.py)
        o = base - ROM_BASE
        count, ho, oo, do = struct.unpack_from("<IIII", rom, o)
        hashes = struct.unpack_from(f"<{count}H", rom, o + ho)
        offs = list(struct.unpack_from(f"<{count}I", rom, o + oo))
        later = [b for b in bases if b > base]
        limit = (later[0] if later else len(rom) + ROM_BASE) - base
        ents = []
        for i, (h, off) in enumerate(zip(hashes, offs)):
            end = offs[i + 1] if i + 1 < count else limit
            ents.append((h, base + off, end - off))
        label, t = type_label(key)
        out.append((label, t, base, ents))
    return out


def main():
    rom = load_rom()
    cmd = sys.argv[1] if len(sys.argv) > 1 else "list"
    arcs = archives(rom)
    if cmd == "list":
        print(f"{'type':6} {'hash':6} {'archive':10} {'count':>5} {'bytes':>9}")
        for label, t, base, ents in arcs:
            size = sum(s for _, _, s in ents)
            print(f"{label:6} {t or 0:04X}   {base:08X} {len(ents):5} {size:9}")
    elif cmd == "entries":
        for label, t, base, ents in arcs:
            if label == sys.argv[2]:
                for h, a, s in ents:
                    magic = rom[a - ROM_BASE:a - ROM_BASE + 8].hex()
                    print(f"{h:04X} {a:08X} {s:7} {magic}")
    elif cmd == "extract":
        want = set(sys.argv[2:])
        for label, t, base, ents in arcs:
            if want and label not in want:
                continue
            d = os.path.join(ROOT, "build", "assets", "archives", label)
            os.makedirs(d, exist_ok=True)
            for h, a, s in ents:
                with open(os.path.join(d, f"{h:04X}.bin"), "wb") as f:
                    f.write(rom[a - ROM_BASE:a - ROM_BASE + s])
            print(f"{label}: {len(ents)} files -> {os.path.relpath(d, ROOT)}")


if __name__ == "__main__":
    main()
