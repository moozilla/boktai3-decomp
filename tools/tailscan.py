#!/usr/bin/env python3
"""Find disassembler "functions" that are really tails of the function before them.

  tailscan.py            list candidates not yet in symbols/function_nonentries.csv

agbcc reaches distant code in a very large function with a far branch (`bl _label`),
and gbadisasm takes each such `bl` target for a new function. The pieces have no
prologue, read the owner's stack frame and leave through its epilogue, so nobody can
match either half until they are merged. A candidate here:
  * does not start with `push`;
  * is never named in a `.4byte` (no function pointer);
  * is called only by `bl` from between the nearest preceding pushing function
    (the owner) and itself.
Review each one (first lines, how it exits), then add the address to
symbols/function_nonentries.csv; the split then keeps it inside its owner.
Reads gen/code.s, so run it after a build. It also lists INCLUDE_ASM lines in src/ that
still name a recorded non-entry; delete those lines (the owner's asm now contains the code).
"""
import csv
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402

FUNC = re.compile(r"\t(?:non_word_aligned_thumb|thumb|arm)_func_start (\S+)")
LABEL = re.compile(r"(\w+): @ 0x([0-9A-F]{8})")


def main():
    lines = open(os.path.join(ROOT, "gen", "code.s")).read().split("\n")
    funcs = []
    for i, line in enumerate(lines):
        m = FUNC.match(line)
        if m and i + 1 < len(lines):
            label = LABEL.match(lines[i + 1])
            if label:
                funcs.append((m.group(1), int(label.group(2), 16), i))
    starts = {f[2]: f for f in funcs}
    callers, pointers, current = {}, set(), None
    for i, line in enumerate(lines):
        current = starts.get(i, current)
        m = re.match(r"\tbl (\S+)", line)
        if m and current:
            callers.setdefault(m.group(1), []).append(current[1])
        pointers.update(re.findall(r"\.4byte (sub_\w+)", line))
    path = os.path.join(ROOT, "symbols", "function_nonentries.csv")
    known = {int(r["address"], 16) for r in csv.DictReader(open(path))}

    def first(i):
        for line in lines[i + 2:i + 12]:
            if line.startswith("\t") and not line.startswith("\t."):
                return line.strip()
        return ""

    owner = None
    for name, address, i in funcs:
        insn = first(i)
        if insn.startswith("push"):
            owner = (name, address)
            continue
        calls = callers.get(name, [])
        if (owner and address not in known and calls and name not in pointers
                and all(owner[1] <= c < address for c in calls)):
            print(f"{address:08X} {name:16} owner={owner[0]} callers={len(calls)}  {insn}")


def stale_includes():
    path = os.path.join(ROOT, "symbols", "function_nonentries.csv")
    names = {"sub_" + r["address"].upper() for r in csv.DictReader(open(path))}
    for src in sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True)):
        for n, line in enumerate(open(src, errors="replace"), 1):
            m = re.search(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', line)
            if m and m.group(1) in names:
                print(f"stale INCLUDE_ASM {os.path.relpath(src, ROOT)}:{n}: {m.group(1)} is a non-entry")


if __name__ == "__main__":
    stale_includes()
    main()
