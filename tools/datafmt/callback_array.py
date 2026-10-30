"""Fixed-size ordered arrays of Thumb callback addresses.

CSV columns: index (decimal), callback (hex with the Thumb bit), name
(informational; ignored by encode). Null entries are allowed. Array order is
semantic: the actor command dispatcher indexes these tables by command[0].
Every non-null callback is emitted as a relocatable pointer by symbolize.py.
"""
import csv
import io
import struct

SOURCE = "decoded"
ROM_BASE = 0x08000000
ROM_LIMIT = 0x0A000000


def _count(start, end):
    if start % 4 or end < start or (end - start) % 4:
        raise ValueError("callback_array: range must contain aligned words")
    return (end - start) // 4


def decode(rom, start, end, names):
    count = _count(start, end)
    offset = start - ROM_BASE
    if offset < 0 or offset + count * 4 > len(rom):
        raise ValueError("callback_array: range lies outside ROM")
    out = io.StringIO()
    writer = csv.writer(out, lineterminator="\n")
    writer.writerow(["index", "callback", "name"])
    for index in range(count):
        callback = struct.unpack_from("<I", rom, offset + index * 4)[0]
        name = names.get(callback & ~1, f"sub_{callback & ~1:08X}") if callback else ""
        writer.writerow([index, f"{callback:08X}", name])
    return out.getvalue()


def encode(text, start, end):
    count = _count(start, end)
    reader = csv.DictReader(io.StringIO(text))
    if reader.fieldnames is None or not {"index", "callback"}.issubset(reader.fieldnames):
        raise ValueError("callback_array: CSV requires index and callback columns")
    rows = list(reader)
    if len(rows) != count:
        raise ValueError(f"callback_array: expected {count} entries, got {len(rows)}")
    items = []
    for expected, row in enumerate(rows):
        if int(row["index"], 10) != expected:
            raise ValueError("callback_array: indices must be consecutive and in table order")
        callback = int(row["callback"], 16)
        if callback and (not ROM_BASE <= callback < ROM_LIMIT or not callback & 1):
            raise ValueError(f"callback_array: entry {expected} is not null or a Thumb ROM callback")
        items.append((4, callback, bool(callback)))
    return items
