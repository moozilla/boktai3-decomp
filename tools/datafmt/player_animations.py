"""Four fixed player animation-variant tables in mode/index order.

ActorVariant_GetTableValue selects modes 0, 1, 2 and 6, each with 58 u16
entries. Player_SetAnimation adds the folded heading to the returned base
sequence. FFFF entries are preserved as numbers, without interpreting them.
"""
import csv
import io
import struct

SOURCE = "decoded"
ROM_BASE = 0x08000000
MODES = (0, 1, 2, 6)
SLOTS = 58
TABLE_BYTES = SLOTS * 2
SIZE = len(MODES) * TABLE_BYTES


def _range(start, end):
    if start % 2 or end - start != SIZE:
        raise ValueError(f"player_animations: range must contain four {SLOTS}-entry halfword tables")


def decode(rom, start, end, names):
    _range(start, end)
    offset = start - ROM_BASE
    if offset < 0 or offset + SIZE > len(rom):
        raise ValueError("player_animations: range lies outside ROM")
    out = io.StringIO()
    writer = csv.writer(out, lineterminator="\n")
    writer.writerow(["mode", "index", "base_sequence"])
    for table, mode in enumerate(MODES):
        values = struct.unpack_from(f"<{SLOTS}H", rom, offset + table * TABLE_BYTES)
        writer.writerows((mode, index, f"{value:04X}") for index, value in enumerate(values))
    return out.getvalue()


def parse(text, start, end):
    _range(start, end)
    reader = csv.DictReader(io.StringIO(text))
    if reader.fieldnames is None or not {"mode", "index", "base_sequence"}.issubset(reader.fieldnames):
        raise ValueError("player_animations: CSV requires mode, index and base_sequence columns")
    rows = list(reader)
    if len(rows) != len(MODES) * SLOTS:
        raise ValueError(f"player_animations: expected {len(MODES) * SLOTS} entries, got {len(rows)}")
    out = []
    for position, row in enumerate(rows):
        mode, index = int(row["mode"], 10), int(row["index"], 10)
        if (mode, index) != (MODES[position // SLOTS], position % SLOTS):
            raise ValueError("player_animations: rows must retain mode and index order")
        value = int(row["base_sequence"], 16)
        if not 0 <= value <= 0xFFFF:
            raise ValueError(f"player_animations: mode {mode}, index {index} is not a u16")
        out.append({"mode": mode, "index": index, "base_sequence": value})
    return out


def encode(text, start, end):
    return [(2, row["base_sequence"], False) for row in parse(text, start, end)]
