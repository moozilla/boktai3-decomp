"""Editable player animation variants and their direct cue-code consumers."""
import csv
import os

from datafmt import load_manifest
from datafmt.player_animations import MODES, SLOTS, TABLE_BYTES, parse

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
LOOKUP = 0x081419D4
SELECT = 0x0813AE18
CUE_RANGES = ((0x0814F804, 0x08156BBC), (0x0815AD24, 0x0815C9EC),
              (0x0815D688, 0x0815E034))


def _names():
    with open(os.path.join(ROOT, "symbols", "functions.csv"), newline="") as source:
        return {int(row["addr"], 16): row["name"] for row in csv.DictReader(source)}


def animation_variants():
    """One manifest-backed table; caller links come from ROM disassembly,
    while display names follow the current symbols rather than CSV snapshots.
    Read the small editable CSV on each request so source edits are visible.
    """
    import actors
    entry = next((entry for entry in load_manifest() if entry.format == "player_animations"), None)
    if entry is None:
        return None
    with open(entry.file, encoding="utf-8") as source:
        rows = parse(source.read(), entry.start, entry.end)
    names = _names()

    def function(addr):
        return {"addr": f"{addr:08X}", "name": names.get(addr, f"sub_{addr:08X}")}

    funcs, generated_names = actors.code()
    lookup_names = {name for name, addr in generated_names.items() if addr == LOOKUP}
    consumers = [function(addr) for addr in sorted(funcs)
                 if any(start <= addr < end for start, end in CUE_RANGES)
                 and lookup_names.intersection(actors.calls(addr))]
    tables = []
    for table, mode in enumerate(MODES):
        start = entry.start + table * TABLE_BYTES
        tables.append({"mode": mode, "addr": f"{start:08X}",
                       "end": f"{start + TABLE_BYTES:08X}", "size": TABLE_BYTES,
                       "entries": rows[table * SLOTS:(table + 1) * SLOTS]})
    return {"name": entry.name, "addr": f"{entry.start:08X}", "end": f"{entry.end:08X}",
            "size": entry.end - entry.start, "source": os.path.relpath(entry.file, ROOT),
            "description": entry.description, "lookup": function(LOOKUP), "select": function(SELECT),
            "tables": tables, "consumers": consumers}
