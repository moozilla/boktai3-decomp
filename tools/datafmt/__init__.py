"""Decoded ROM data: formats that turn a ROM range into an editable source file
and back into bytes.

data/manifest.csv lists the decoded ranges:

    name,start,end,format,file,description

Each format module (tools/datafmt/<format>.py) provides

    decode(rom, start, end, names) -> str     # ROM bytes -> source text
    encode(text, start, end) -> [Item]        # source text -> items

An Item is (size, value, pointer): size 1, 2 or 4 bytes; with pointer=True the
4-byte value is a ROM address that tools/symbolize.py writes as a symbol, so
the range stays shiftable. With pointer="incbin", value is a file path and
size its length (emitted as .incbin). A module may instead provide
extract(rom, start, end, path) and encode_path(path, start, end) when its
source is more than one file. `rom` is the whole ROM image (index 0 = 0x08000000);
`names` maps function addresses to their current names (informational columns).

tools/datafmt/check.py verifies that every manifest entry re-encodes to the
original bytes. Only structured tables are committed under data/; graphics,
text and audio extract to the ignored build/ tree (see docs/DATA.md). A
manifest file under build/ is extracted from the ROM when missing. A format
module with RESIZABLE = True may encode to a different size; later data shifts.
SOURCE says how the build's source holds the bytes: "decoded" (an editable
form such as CSV rows or text strings) or "split" (the range's structure is
regenerated but its pieces are opaque extracted files). It is informational;
the ROM data measure is the verified round-trip (tools/data_roundtrip.py,
docs/DATA_TRACKING.md).
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

    def extracted(self):
        """True for ROM-derived sources kept under build/ (never committed)."""
        return os.path.relpath(self.file, ROOT).startswith("build" + os.sep)

    def encode(self):
        if not os.path.exists(self.file) and self.extracted():
            self.extract()
        mod = self.module()
        if hasattr(mod, "encode_path"):
            items = mod.encode_path(self.file, self.start, self.end)
        else:
            with open(self.file, encoding="utf-8") as f:
                items = mod.encode(f.read(), self.start, self.end)
        size = sum(s for s, _, _ in items)
        if size != self.end - self.start and not getattr(self.module(), "RESIZABLE", False):
            raise ValueError(f"{self.name}: encodes to {size:#x} bytes, range is {self.end - self.start:#x}")
        return items


    def extract(self):
        import sys
        sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
        from romlib import load_rom
        os.makedirs(os.path.dirname(self.file), exist_ok=True)
        mod = self.module()
        # write then rename: make -j runs symbolize twice, and the other run may read this file
        tmp = f"{self.file}.{os.getpid()}.tmp"
        if hasattr(mod, "extract"):
            mod.extract(load_rom(), self.start, self.end, tmp)
        else:
            with open(tmp, "w", encoding="utf-8") as f:
                f.write(mod.decode(load_rom(), self.start, self.end, {}))
        os.replace(tmp, self.file)


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
    for size, value, kind in items:
        if kind == "incbin":
            with open(value, "rb") as f:
                out += f.read()
            continue
        out += (value & ((1 << (8 * size)) - 1)).to_bytes(size, "little")
    return bytes(out)
