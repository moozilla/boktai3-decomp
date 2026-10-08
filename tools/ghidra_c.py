#!/usr/bin/env python3
"""Print Ghidra's decompiler output for a function (a rough first draft only).

  ghidra_c.py ADDR
Needs the export from tools/ghidra/README.md at gen/ghidra/decomp.c.
"""
import os
import sys

from romlib import ROOT

addr = sys.argv[1].upper().replace("0X", "").rjust(8, "0")
path = os.path.join(ROOT, "gen", "ghidra", "decomp.c")
out = []
take = False
for ln in open(path, encoding="utf-8", errors="replace"):
    if ln.startswith("// ===== "):
        if take:
            break
        take = ln.startswith(f"// ===== {addr} ")
    if take:
        out.append(ln)
print("".join(out) if out else f"no Ghidra function at {addr}")
