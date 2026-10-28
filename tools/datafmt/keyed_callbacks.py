"""Sorted (u32 key, Thumb callback) records ending in an all-zero sentinel.

The script `chara` command looks keys up by binary search (08225844), so rows
must stay sorted by key. Keys are 16-bit string hashes (sub_08219BD8:
h = rotl16(h, 5) + c), the scheme MGS's GCL uses for its `chara` names.

CSV columns: key (hex), callback (hex address, Thumb bit set), name (the
callback's current function name; informational, ignored by encode).
"""
import csv
import io
import struct

# how the source holds the bytes (tools/progress.py ROM data measure, docs/DATA_TRACKING.md)
SOURCE = "decoded"


def decode(rom, start, end, names):
    out = io.StringIO()
    w = csv.writer(out, lineterminator="\n")
    w.writerow(["key", "callback", "name"])
    o = start - 0x08000000
    while o < end - 0x08000000 - 8:
        key, cb = struct.unpack_from("<II", rom, o)
        w.writerow([f"{key:04X}", f"{cb:08X}", names.get(cb & ~1, f"sub_{cb & ~1:08X}")])
        o += 8
    if rom[o:o + 8] != bytes(8):
        raise ValueError("keyed_callbacks: no zero sentinel at the end of the range")
    return out.getvalue()


def encode(text, start, end):
    rows = list(csv.DictReader(io.StringIO(text)))
    keys = [int(r["key"], 16) for r in rows]
    if keys != sorted(keys) or len(set(keys)) != len(keys):
        raise ValueError("keyed_callbacks: keys must be unique and sorted (binary search)")
    items = []
    for r, key in zip(rows, keys):
        cb = int(r["callback"], 16)
        if not cb & 1:
            raise ValueError(f"keyed_callbacks: callback {cb:08X} for key {key:04X} is not Thumb")
        items += [(4, key, False), (4, cb, True)]
    return items + [(4, 0, False), (4, 0, False)]
