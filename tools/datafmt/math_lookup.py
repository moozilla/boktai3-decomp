"""Numeric lookup tables used by movement, particle emitters and the angle routines."""
import csv
import io
import struct

SOURCE = "decoded"
ROM_BASE = 0x08000000

# Each row keeps its physical index, including the extra quarter-turn of sine.
TABLES = {
    0x08605E48: ("I", 8, ("phase", "rate"), (0x0809CC8C,),
                 "Lun selects a rate by shifting route progress right by seven and capping the table index at seven."),
    0x086141BC: ("H", 1024, ("index", "sample"), (0x081EE130, 0x0818B2A0),
                 "The effect RNG cursor at 03005304 wraps modulo 1024 in this fixed ROM bank."),
    0x086149C4: ("h", 320, ("phase", "sine_q12"), (0x082362EC, 0x08128F98, 0x08047068),
                 "One turn is 256 steps. Divide by 4096 for the sine. The extra quarter-turn allows a cosine lookup at phase + 64."),
    0x08614C44: ("B", 257, ("ratio_q8", "angle"), (0x082215E4,),
                 "Math_Atan2 indexes by the smaller absolute component divided by the larger, scaled by 256. The result spans 0 to 32 angle steps."),
}


def table_schema(start, end):
    if start not in TABLES:
        raise ValueError("unknown math lookup table")
    code, count, columns, consumers, meaning = TABLES[start]
    item = struct.Struct("<" + code)
    if end - start != count * item.size:
        raise ValueError("incorrect math lookup extent")
    return dict(item=item, count=count, columns=columns, consumers=consumers, meaning=meaning)


def parse_rows(text, start, end):
    schema = table_schema(start, end)
    index, value = schema["columns"]
    reader = csv.DictReader(io.StringIO(text))
    if reader.fieldnames != [index, value]:
        raise ValueError(f"expected {index},{value}")
    rows = []
    for i, row in enumerate(reader):
        if None in row or row[index] is None or row[value] is None or int(row[index]) != i:
            raise ValueError(f"invalid or missing lookup row {i}")
        sample = int(row[value])
        try:
            schema["item"].pack(sample)
        except struct.error as err:
            raise ValueError(f"lookup row {i} is out of range") from err
        rows.append({index: i, value: sample})
    if len(rows) != schema["count"]:
        raise ValueError(f'expected {schema["count"]} lookup rows')
    return rows


def decode(rom, start, end, names):
    schema = table_schema(start, end)
    view = memoryview(rom)[start - ROM_BASE:end - ROM_BASE]
    if len(view) != end - start:
        raise ValueError("lookup extends beyond ROM")
    output = io.StringIO()
    writer = csv.writer(output, lineterminator="\n")
    writer.writerow(schema["columns"])
    writer.writerows((i, n) for i, (n,) in enumerate(schema["item"].iter_unpack(view)))
    return output.getvalue()


def encode(text, start, end):
    schema = table_schema(start, end)
    size = schema["item"].size
    value = schema["columns"][1]
    return [(size, row[value] % (1 << (8 * size)), False)
            for row in parse_rows(text, start, end)]
