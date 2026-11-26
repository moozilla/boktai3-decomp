#!/usr/bin/env python3
"""Generate one checksummed battery fixture with overrides for port and ROM."""
import json
from pathlib import Path
import struct
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/assets"))
import saves


def build(source, output, options):
    save = saves.load(str(source))
    current = saves.current(save)
    if current is None:
        raise ValueError("The base battery has no valid checksum")
    regions = {name: bytearray(current[name]) for name in saves.REGIONS}
    v, g = regions["vars"], regions["game"]
    if "story" in options:
        struct.pack_into("<h", v, 0x3C, options["story"])
    if "entrance" in options:
        struct.pack_into("<h", v, 0x70, options["entrance"])
    if "scene" in options:
        struct.pack_into("<I", g, 0x5A4, options["scene"])
    for flag in options.get("flags", []):
        bank, offset, size, bit, field, value = (flag[k] for k in ("bank", "offset", "size", "bit", "type", "value"))
        if bank == "r708":
            target = v
        elif bank == "gs":
            target = g
            if offset >= len(g):
                offset -= len(g)
                target = regions["game2"]
        else:
            raise ValueError(f"{bank} is not a battery-backed variable bank")
        if offset < 0 or offset + size > len(target):
            raise ValueError("Script variable crosses a battery-region boundary")
        if field == 4:
            mask = 1 << bit
            target[offset] = (target[offset] & ~mask) | (mask if value else 0)
        else:
            target[offset:offset + size] = (value & ((1 << (size * 8)) - 1)).to_bytes(size, "little")
    saves.write_sav({name: bytes(data) for name, data in regions.items()}, output, save.get("header"))


if __name__ == "__main__":
    build(Path(sys.argv[1]), Path(sys.argv[2]), json.load(sys.stdin))
