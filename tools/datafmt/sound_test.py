"""Ordered song numbers shown by the sound-test menu. The final -1 ends each list."""
import csv
import io
import struct

SOURCE = "decoded"
ROM_BASE = 0x08000000
LISTS = {
    0x086136F0: ("music", 94),
    0x086137AE: ("effects", 522),
    0x08613BC4: ("voices", 150),
}


def count(start, end):
    if start not in LISTS or end != start + 2 * (LISTS[start][1] + 1):
        raise ValueError("unexpected sound-test list extent")
    return LISTS[start][1]


def songs(text, start, end):
    expected = count(start, end)
    reader = csv.DictReader(io.StringIO(text))
    if reader.fieldnames != ["number", "song"]:
        raise ValueError("expected number,song")
    result = []
    for i, row in enumerate(reader, 1):
        if None in row or None in row.values() or int(row["number"]) != i:
            raise ValueError(f"invalid sound-test row {i}")
        song = int(row["song"])
        if not 0 <= song <= 0x7fff:
            raise ValueError(f"invalid song number in row {i}")
        result.append(song)
    if len(result) != expected:
        raise ValueError(f"expected {expected} songs")
    return result


def decode(rom, start, end, names):
    size = count(start, end)
    values = struct.unpack_from(f"<{size + 1}h", rom, start - ROM_BASE)
    if values[-1] != -1 or any(v < 0 for v in values[:-1]):
        raise ValueError("invalid sound-test terminator")
    out = io.StringIO()
    writer = csv.writer(out, lineterminator="\n")
    writer.writerow(("number", "song"))
    writer.writerows(enumerate(values[:-1], 1))
    return out.getvalue()


def encode(text, start, end):
    return [(2, song, False) for song in songs(text, start, end)] + [(2, 0xffff, False)]
