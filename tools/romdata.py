#!/usr/bin/env python3
"""ROM data progress: the data region split into report units by kind
(docs/DATA_TRACKING.md). No ROM needed.

  romdata.py             per-kind summary

symbols/rom_data.csv lists the units: contiguous half-open ranges covering
ROM_DATA_START-ROM_DATA_END, each with a kind, a name, and the bytes whose
format is identified (decoded by a tool or the Studio; reviewed, not a match).

A byte is **matched** when a project tool decodes it to an editable,
meaningful form and re-encodes that form to the same bytes, verified by
tools/data_roundtrip.py against a local ROM and recorded (ranges and tool
names only) in symbols/data_roundtrip.csv. The build need not use that form.
For LZ77 blocks the decompressed content must round-trip; whether
recompression is byte-exact is reported separately (level lz-exact).

GCL scripts (kind "scripts") are matched per procedure instead, from
gcl/progress.csv: "matched" = decompiles and recompiles byte-exact and is
named; "decompiled" = round-trips but unnamed (counted apart, not matched).
"""
import csv
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT / "tools") not in sys.path:
    sys.path.insert(0, str(ROOT / "tools"))

ROM_DATA_START = 0x0824DAFC  # first byte of gen/data.s (after the code's alignment)
ROM_DATA_END = 0x08E88144    # end of used ROM; the zero padding after it is free space
UNITS = ROOT / "symbols/rom_data.csv"
MANIFEST = ROOT / "data/manifest.csv"
GCL_PROGRESS = ROOT / "gcl/progress.csv"
ROUNDTRIP = ROOT / "symbols/data_roundtrip.csv"
SYMBOLS = ROOT / "symbols/data.csv"

# report category id, label, in report order
KINDS = [("graphics", "Graphics"), ("palettes", "Palettes"), ("sound", "Sound"), ("text", "Text"),
         ("actors", "Actors"), ("tables", "Tables (code rodata)"), ("scripts", "Scripts (GCL)"),
         ("other", "Other/unknown")]
CATEGORY_PREFIX = "rom_"
LABEL_PREFIX = "ROM data: "


def load_units(path=UNITS):
    with open(path, newline="", encoding="utf-8") as f:
        rows = [{"start": int(r["start"], 16), "end": int(r["end"], 16), "kind": r["kind"],
                 "unit": r["unit"], "identified": int(r["identified"])} for r in csv.DictReader(f)]
    kinds = {k for k, _ in KINDS}
    pos = ROM_DATA_START
    for r in rows:
        if r["start"] != pos:
            raise ValueError(f"{path}: {r['unit']} starts at {r['start']:08X}, expected {pos:08X}")
        if r["end"] <= r["start"] or r["kind"] not in kinds or not 0 <= r["identified"] <= r["end"] - r["start"]:
            raise ValueError(f"{path}: bad row {r['unit']}")
        pos = r["end"]
    if pos != ROM_DATA_END:
        raise ValueError(f"{path}: units end at {pos:08X}, expected {ROM_DATA_END:08X}")
    return rows


def load_roundtrip(path=ROUNDTRIP):
    """[(start, end, what, tool, level)] from the round-trip manifest."""
    if not Path(path).exists():
        return []
    with open(path, newline="", encoding="utf-8") as f:
        rows = csv.DictReader(ln for ln in f if not ln.startswith("#"))
        return [(int(r["start"], 16), int(r["end"], 16), r["what"], r["tool"], r["level"]) for r in rows]


def load_gcl(path=GCL_PROGRESS):
    """([matched spans], [decompiled-only spans]) of GCL procedures."""
    matched, decompiled = [], []
    if Path(path).exists():
        with open(path, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f):
                a = int(r["start"], 16)
                {"matched": matched, "decompiled": decompiled}[r["status"]].append((a, a + int(r["size"])))
    return matched, decompiled


def union(spans):
    out = []
    for a, b in sorted(spans):
        if out and a <= out[-1][1]:
            out[-1][1] = max(out[-1][1], b)
        else:
            out.append([a, b])
    return out


def overlap(spans, a, b):
    return sum(max(0, min(b, y) - max(a, x)) for x, y in spans)


def measure(units=UNITS, roundtrip=ROUNDTRIP, gcl=GCL_PROGRESS):
    """Units with size, matched, identified, GCL-decompiled bytes, and the
    LZ77 split of their matched bytes (lz_exact, lz_content)."""
    rt = load_roundtrip(roundtrip)
    data = union((s, e) for s, e, *_ in rt)
    lz_exact = union((s, e) for s, e, _, _, lv in rt if lv == "lz-exact")
    lz_content = union((s, e) for s, e, _, _, lv in rt if lv == "lz-content")
    gm, gd = load_gcl(gcl)
    gm, gd = union(gm), union(gd)
    out = []
    for r in load_units(units):
        a, b = r["start"], r["end"]
        if r["kind"] == "scripts":
            m, dec = overlap(gm, a, b), overlap(gd, a, b)
        else:
            m, dec = overlap(data, a, b), 0
        out.append({**r, "size": b - a, "matched": m, "decompiled": dec,
                    "lz_exact": overlap(lz_exact, a, b), "lz_content": overlap(lz_content, a, b),
                    "label": dict(KINDS)[r["kind"]]})
    return out


def named_symbols(path=SYMBOLS):
    with open(path, newline="", encoding="utf-8") as f:
        return sum(ROM_DATA_START <= int(r["addr"], 16) < ROM_DATA_END for r in csv.DictReader(f))


def by_kind(units):
    """[(kind, label, size, matched, identified, decompiled)] in KINDS order."""
    out = []
    for k, label in KINDS:
        us = [u for u in units if u["kind"] == k]
        out.append((k, label, sum(u["size"] for u in us), sum(u["matched"] for u in us),
                    sum(u["identified"] for u in us), sum(u["decompiled"] for u in us)))
    return out


def main():
    units = measure()
    total = sum(u["size"] for u in units)
    for k, label, size, m, ident, dec in by_kind(units):
        print(f"{label:22} {m:>9,} / {size:>10,} matched ({100 * m / size:5.1f}%), "
              f"identified {100 * ident / size:5.1f}%")
    m = sum(u["matched"] for u in units)
    print(f"{'ROM data':22} {m:>9,} / {total:>10,} matched ({100 * m / total:5.2f}%)")


if __name__ == "__main__":
    main()
