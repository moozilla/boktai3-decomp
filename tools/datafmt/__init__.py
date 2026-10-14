"""Decoded ROM data: formats that turn a ROM range into an editable source file
and back into bytes.

data/manifest.csv lists the decoded ranges:

    name,start,end,format,file,description

Each format module (tools/datafmt/<format>.py) provides

    decode(rom, start, end, names) -> str     # ROM bytes -> source text
    encode(text, start, end) -> [Item]        # source text -> items

An Item is (size, value, pointer): size 1, 2 or 4 bytes; with pointer=True the
4-byte value is a ROM address that tools/symbolize.py writes as a symbol, so
the range stays shiftable. `rom` is the whole ROM image (index 0 = 0x08000000);
`names` maps function addresses to their current names (informational columns).

tools/datafmt/check.py verifies that every manifest entry re-encodes to the
original bytes. Only structured tables are committed under data/; graphics,
text and audio extract to the ignored build/ tree (see docs/DATA.md).
"""
import csv
import importlib
import os

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ROM_BASE = 0x08000000
MANIFEST = os.path.join(ROOT, "data", "manifest.csv")


class Entry:
    def __init__(self, row):
        self.name = row["name"]
        self.start = int(row["start"], 16)
        self.end = int(row["end"], 16)
        self.format = row["format"]
        self.file = os.path.join(ROOT, row["file"])
        self.description = row.get("description", "")

    def module(self):
        return importlib.import_module("datafmt." + self.format)

    def encode(self):
        with open(self.file, encoding="utf-8") as f:
            items = self.module().encode(f.read(), self.start, self.end)
        size = sum(s for s, _, _ in items)
        if size != self.end - self.start:
            raise ValueError(f"{self.name}: encodes to {size:#x} bytes, range is {self.end - self.start:#x}")
        return items


def load_manifest():
    if not os.path.exists(MANIFEST):
        return []
    with open(MANIFEST, newline="") as f:
        entries = sorted((Entry(r) for r in csv.DictReader(f)), key=lambda e: e.start)
    for a, b in zip(entries, entries[1:]):
        if b.start < a.end:
            raise ValueError(f"data/manifest.csv: {a.name} overlaps {b.name}")
    return entries


def to_bytes(items):
    out = bytearray()
    for size, value, _ in items:
        out += (value & ((1 << (8 * size)) - 1)).to_bytes(size, "little")
    return bytes(out)
