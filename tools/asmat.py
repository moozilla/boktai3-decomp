#!/usr/bin/env python3
"""Print the generated disassembly around a ROM address: asmat.py ADDR [LINES]

Starts at the function containing ADDR. Reviewed interior labels
(symbols/function_nonentries.csv) are not function starts: the listing backs up
past them to the real owner and prints them as plain labels, as the split does.
"""
import re
import signal
import sys

from romlib import ROOT
from split import function_nonentries

signal.signal(signal.SIGPIPE, signal.SIG_DFL)  # `| head` must not raise BrokenPipeError

FUNC_START = re.compile(r"^\t(?:thumb|arm|non_word_aligned_thumb)_func_start (\S+)")
LABEL = re.compile(r"^(?:\w+): @ 0x([0-9A-F]{8})|^_([0-9A-F]{8}):")

addr = int(sys.argv[1], 16)
n = int(sys.argv[2]) if len(sys.argv) > 2 else 40
lines = open(ROOT + "/gen/code.s").read().split("\n")
nonentries = function_nonentries()


def label_addr(i):
    m = LABEL.match(lines[i]) if i < len(lines) else None
    return int(m.group(1) or m.group(2), 16) if m else None


def is_interior_start(i):
    """A func_start line whose function label is a reviewed interior label."""
    return bool(FUNC_START.match(lines[i])) and label_addr(i + 1) in nonentries


best = None
for i, ln in enumerate(lines):
    a = label_addr(i)
    if a is not None:
        if a <= addr:
            best = i
        else:
            break
# back up to the containing function start, skipping interior labels
i = best
while i > 0 and not (FUNC_START.match(lines[i]) and not is_interior_start(i)):
    i -= 1
out = []
for j in range(i, max(best + n, i + n)):
    if j >= len(lines):
        break
    if j > i and is_interior_start(j):
        out.append(f"\t@ interior label (symbols/function_nonentries.csv), not a function start")
        continue
    out.append(lines[j])
print("\n".join(out))
