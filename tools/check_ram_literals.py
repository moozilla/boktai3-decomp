#!/usr/bin/env python3
"""Check `#define NAME 0xADDR /* SYMBOL */` literals in include/ against the link map.

Literal RAM addresses are sometimes needed for matching (docs/REGALLOC.md).
Each must name its symbol; this fails the build if the symbol's address in
build/link.ld differs, so a literal can't silently drift from the symbol.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFINE = re.compile(r"^#define\s+(\w+)\s+(0x[0-9A-Fa-f]+)\s*/\*\s*(\w+)\s*\*/", re.M)
link = (ROOT / "build/link.ld").read_text()
addrs = {m.group(1): int(m.group(2), 16) for m in re.finditer(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", link, re.M)}
bad = 0
for h in sorted((ROOT / "include").rglob("*.h")):
    for name, value, sym in DEFINE.findall(h.read_text()):
        if sym not in addrs:
            print(f"{h.relative_to(ROOT)}: {name}: symbol {sym} not in build/link.ld")
            bad += 1
        elif addrs[sym] != int(value, 16):
            print(f"{h.relative_to(ROOT)}: {name} = {value} but {sym} = 0x{addrs[sym]:08X}")
            bad += 1
sys.exit(1 if bad else 0)
