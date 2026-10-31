"""The three fixed numeric tables used to initialize and launch marker fragments.

CSV rows keep the original group and fragment order. Coordinates use signed
halfword values; the fourth halfword remains an uninterpreted unsigned value.
Every encoded item is numeric, including frame bytes that resemble a pointer.
"""
import csv
import io
import struct

SOURCE = "decoded"
ROM_BASE = 0x08000000

TABLES = {
    0x0824E6D4: {
        "end": 0x0824E6E6,
        "columns": ("facing", "fragment", "frame"),
        "format": "B",
        "consumers": (0x08180CE0,),
        "meaning": "Nine sprite frames per facing group: group 0 for quantized directions 1/2, group 1 for 0/3.",
    },
    0x0824E6E6: {
        "end": 0x0824E6F8,
        "columns": ("set", "fragment", "animation"),
        "format": "B",
        "consumers": (0x08180CE0,),
        "meaning": "Set 0 is used by marker kinds 3/5/7/9/13/15; set 1 by the other kinds. Values select embedded animation 0 or 1.",
    },
    0x0824E818: {
        "end": 0x0824E8A8,
        "columns": ("facing", "fragment", "x", "y", "z", "unknown"),
        "format": "hhhH",
        "consumers": (0x08181514,),
        "meaning": "Nine XYZ offsets per facing group: group 0 for quantized directions 1/2, group 1 for 0/3. The fourth halfword is preserved without an assigned role.",
    },
}


def table_schema(start, end):
    table = TABLES.get(start)
    if table is None or end != table["end"]:
        raise ValueError("actor_marker_table: expected one of the three reviewed fixed ranges")
    return table


def parse_rows(text, start, end):
    table = table_schema(start, end)
    reader = csv.DictReader(io.StringIO(text))
    if reader.fieldnames != list(table["columns"]):
        raise ValueError("actor_marker_table: expected columns " + ",".join(table["columns"]))
    rows = list(reader)
    if len(rows) != 18:
        raise ValueError("actor_marker_table: expected exactly two groups of nine fragments")
    result = []
    group = table["columns"][0]
    for index, row in enumerate(rows):
        if None in row or any(value is None for value in row.values()):
            raise ValueError("actor_marker_table: every row must have the declared columns")
        try:
            values = {key: int(value, 10) for key, value in row.items()}
        except ValueError as error:
            raise ValueError("actor_marker_table: values must be decimal integers") from error
        if (values[group], values["fragment"]) != divmod(index, 9):
            raise ValueError("actor_marker_table: keep group and fragment indices in physical order")
        for key, code in zip(table["columns"][2:], table["format"]):
            low, high = {"B": (0, 255), "h": (-32768, 32767), "H": (0, 65535)}[code]
            if not low <= values[key] <= high:
                raise ValueError(f"actor_marker_table: {key} is outside {low}..{high}")
        if "animation" in values and values["animation"] > 1:
            raise ValueError("actor_marker_table: animation must select one of the two embedded animations")
        result.append(values)
    return result


def decode(rom, start, end, names):
    table = table_schema(start, end)
    offset = start - ROM_BASE
    if offset < 0 or end - ROM_BASE > len(rom):
        raise ValueError("actor_marker_table: range lies outside ROM")
    out = io.StringIO()
    writer = csv.writer(out, lineterminator="\n")
    writer.writerow(table["columns"])
    stride = struct.calcsize("<" + table["format"])
    for index in range(18):
        values = struct.unpack_from("<" + table["format"], rom, offset + index * stride)
        writer.writerow((*divmod(index, 9), *values))
    return out.getvalue()


def encode(text, start, end):
    table = table_schema(start, end)
    items = []
    for row in parse_rows(text, start, end):
        for key, code in zip(table["columns"][2:], table["format"]):
            size = 1 if code == "B" else 2
            items.append((size, row[key] & ((1 << (size * 8)) - 1), False))
    return items
