#!/usr/bin/env python3
"""Export the project's symbol names for debuggers and Ghidra.

    python3 tools/symexport.py                    # build/symbols/boktai3.sym and ghidra.tsv
    python3 tools/symexport.py --sym OUT.sym      # no$gba / armips style: one `ADDR NAME` per line
    python3 tools/symexport.py --ghidra OUT.tsv   # input of tools/ghidra/ImportSymbols.java
    python3 tools/symexport.py --no-placeholders  # leave out sub_XXXXXXXX / gUnk_XXXXXXXX names

Sources are symbols/functions.csv (ROM code), symbols/ram.csv (EWRAM/IWRAM
objects) and symbols/data.csv (ROM data regions). Nothing here needs the ROM or a
build. The TSV has the columns

    addr  name  kind  size  mode  confidence  comment

where kind is `function`, `ram` (an object) or `ram-interior` (a label inside
another object) or `data`, size is in hex ("" when unknown) and mode is
`thumb`/`arm` for functions. The comment carries the C declaration for RAM rows.
See docs/RAM.md.
"""
import argparse
import csv
import os
import re
import sys

sys.path.insert(0, os.path.dirname(__file__))
from romlib import ROOT  # noqa: E402

PLACEHOLDER = re.compile(r"^(sub|gUnk|gUnknown|unk)_[0-9A-Fa-f]{8}$")


def rows(name):
    path = os.path.join(ROOT, "symbols", name)
    if not os.path.exists(path):
        return []
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def symbols(placeholders=True):
    """[(addr, name, kind, size, mode, confidence, comment)], sorted by address."""
    out = []
    for r in rows("functions.csv"):
        out.append((r["addr"], r["name"], "function", "", r["mode"], r["confidence"], r["notes"]))
    for r in rows("ram.csv"):
        kind = "ram-interior" if r["kind"] == "interior" else "ram"
        comment = " ".join(x for x in (r["decl"], r["notes"]) if x)
        out.append((r["addr"], r["name"], kind, r["size"], "", r["confidence"], comment))
    for r in rows("data.csv"):
        out.append((r["addr"], r["name"], "data", r["size"], "", r["confidence"], r["notes"]))
    if not placeholders:
        out = [s for s in out if not PLACEHOLDER.match(s[1])]
    out.sort(key=lambda s: (int(s[0], 16), s[1]))
    return out


def clean(s):
    return re.sub(r"\s+", " ", s.replace("\t", " ")).strip()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--sym", help="write a no$gba/armips-style .sym file here")
    ap.add_argument("--ghidra", help="write the TSV for tools/ghidra/ImportSymbols.java here")
    ap.add_argument("--no-placeholders", action="store_true", help="skip sub_XXXXXXXX and gUnk_XXXXXXXX names")
    a = ap.parse_args()
    if not (a.sym or a.ghidra):
        out = os.path.join(ROOT, "build", "symbols")
        a.sym, a.ghidra = os.path.join(out, "boktai3.sym"), os.path.join(out, "ghidra.tsv")
    syms = symbols(not a.no_placeholders)
    for path in (a.sym, a.ghidra):
        if path:
            os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    if a.sym:
        with open(a.sym, "w") as f:
            for addr, name, *_ in syms:
                f.write(f"{addr} {name}\n")
        print(f"{len(syms)} symbols -> {a.sym}")
    if a.ghidra:
        with open(a.ghidra, "w") as f:
            f.write("addr\tname\tkind\tsize\tmode\tconfidence\tcomment\n")
            for s in syms:
                f.write("\t".join(clean(x) for x in s) + "\n")
        print(f"{len(syms)} symbols -> {a.ghidra}")


if __name__ == "__main__":
    main()
