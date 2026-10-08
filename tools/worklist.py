#!/usr/bin/env python3
"""List functions still in asm, with difficulty signals.

  worklist.py [--range START END] [--max-insns N] [--limit K]
  worklist.py --chunks N          split remaining work into N disjoint ranges

Columns: address, name, instructions, calls (bl), branches, jump table?, done?
Functions already in src/ are skipped.  Library ranges (MP2K, libagbsyscall)
are excluded from --chunks; they are lifted from existing sources instead.
"""
import argparse
import glob
import os
import re

from romlib import ROOT
from split import c_functions

LIBRARY = [(0x0822F248, 0x08231440, "m4a (pokeemerald m4a.c)"),
           (0x08248590, 0x082485EC, "libagbsyscall")]


def functions():
    code = open(os.path.join(ROOT, "gen", "code_sym.s")).read().split("\n")
    out = []
    cur = None
    for ln in code:
        m = re.match(r"^\t(?:thumb|arm|non_word_aligned_thumb)_func_start (\S+)", ln)
        if m:
            cur = {"name": m.group(1), "addr": None, "insns": 0, "calls": 0, "branches": 0, "jt": False}
            out.append(cur)
            continue
        if cur is None:
            continue
        m = re.match(r"^\w+: @ 0x([0-9A-F]{8})", ln)
        if m and cur["addr"] is None:
            cur["addr"] = int(m.group(1), 16)
            continue
        s = ln.strip()
        if not s or s.endswith(":") or s.startswith(".") or s.startswith("_0") and ":" in s:
            if "jump table" in s.lower() or s.startswith(".4byte _0"):
                cur["jt"] = True
            continue
        cur["insns"] += 1
        if s.startswith("bl "):
            cur["calls"] += 1
        elif re.match(r"b[a-z]{0,2} ", s):
            cur["branches"] += 1
    return [f for f in out if f["addr"]]


def done_names():
    done = set()
    for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        done |= set(c_functions(p, with_asm=False))
    return done


def in_library(a):
    return any(s <= a < e for s, e, _ in LIBRARY)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--range", nargs=2)
    ap.add_argument("--max-insns", type=int, default=10 ** 9)
    ap.add_argument("--limit", type=int, default=10 ** 9)
    ap.add_argument("--chunks", type=int)
    a = ap.parse_args()
    done = done_names()
    fs = [f for f in functions() if f["name"] not in done]
    if a.chunks:
        work = [f for f in fs if not in_library(f["addr"]) and f["addr"] >= 0x0800145C]
        per = len(work) // a.chunks + 1
        for i in range(a.chunks):
            part = work[i * per:(i + 1) * per]
            if part:
                end = work[(i + 1) * per]["addr"] if (i + 1) * per < len(work) else 0x0824DAFA
                print(f"chunk {i}: {part[0]['addr']:08X} {end:08X}  {len(part)} functions, "
                      f"{sum(f['insns'] for f in part)} instructions")
        return
    lo, hi = (int(a.range[0], 16), int(a.range[1], 16)) if a.range else (0, 1 << 32)
    sel = [f for f in fs if lo <= f["addr"] < hi and f["insns"] <= a.max_insns]
    for f in sel[:a.limit]:
        print(f"{f['addr']:08X}\t{f['name']}\t{f['insns']}\tcalls={f['calls']}\tbr={f['branches']}"
              + ("\tJUMPTABLE" if f["jt"] else ""))


if __name__ == "__main__":
    main()
