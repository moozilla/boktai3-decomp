#!/usr/bin/env python3
"""ROM data progress: the data region split into report units by kind
(docs/DATA_TRACKING.md). No ROM needed.

  romdata.py             per-kind summary

symbols/rom_data.csv lists the units: contiguous half-open ranges covering
ROM_DATA_START-ROM_DATA_END, each with a kind, a name, and the bytes whose
format is identified (decoded by a tool or the Studio; reviewed, not a match).

A byte is **matched** only when the build emits it from an editable, meaningful
source that rebuilds it byte-exact:

* a data/manifest.csv range whose format declares SOURCE = "decoded"
  (tools/datafmt); a "split" format (resource archives: opaque .bin entries)
  earns nothing;
* a GCL procedure that gcl/progress.csv marks "matched" (compiled from GCL
  source and fully annotated); "decompiled" procedures (compiled from source,
  annotation incomplete) are counted separately and earn nothing.
"""
import csv
import importlib
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
SYMBOLS = ROOT / "symbols/data.csv"

# report category id, label, in report order
KINDS = [("graphics", "Graphics"), ("palettes", "Palettes"), ("sound", "Sound"), ("text", "Text"),
         ("tables", "Tables (code rodata)"), ("scripts", "Scripts (GCL)"), ("other", "Other/unknown")]
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


def matched_ranges(manifest=MANIFEST, gcl=GCL_PROGRESS):
    """([start, end) matched spans, [start, end) GCL-decompiled-only spans)."""
    matched, decompiled = [], []
    with open(manifest, newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f):
            mod = importlib.import_module("datafmt." + r["format"])
            if getattr(mod, "SOURCE", "split") == "decoded":
                matched.append((int(r["start"], 16), int(r["end"], 16)))
    if Path(gcl).exists():
        with open(gcl, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f):
                a = int(r["start"], 16)
                span = (a, a + int(r["size"]))
                {"matched": matched, "decompiled": decompiled}[r["status"]].append(span)
    return matched, decompiled


def overlap(spans, a, b):
    return sum(max(0, min(b, y) - max(a, x)) for x, y in spans)


def measure(units=UNITS, manifest=MANIFEST, gcl=GCL_PROGRESS):
    """Units with size, matched, identified and GCL-decompiled bytes."""
    matched, decompiled = matched_ranges(manifest, gcl)
    out = []
    for r in load_units(units):
        size = r["end"] - r["start"]
        m = overlap(matched, r["start"], r["end"])
        out.append({**r, "size": size, "matched": m, "decompiled": overlap(decompiled, r["start"], r["end"]),
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
