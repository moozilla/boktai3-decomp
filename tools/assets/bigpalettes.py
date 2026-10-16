#!/usr/bin/env python3
"""Which sprite palette pool entry (9B1B) the code gives each large sprite.

  bigpalettes.py [HASH...]     print hash, pool entries and the functions

A large sprite (98F5) names no palette. The code that shows one calls
AnimData_Load(anim, hash) (082151E4) and then
AnimData_SelectPaletteBank(anim, pool_index) (08215284), which points the
sprite's bank 0 at pool entry `pool_index` (index < *0x030042E0, the entry at
*0x030042E4 + 32 * index). This scans gen/code.s for those calls with
constant arguments, pairing each palette call with the latest load in the
same function. One sprite can take several entries: nine enemy routines draw
7E7D, each in its own palette (439-453), and 7E82 is the statue (549) and the
armor (551).

Only constant hashes are found (35 sprites); sprites chosen from tables or
script options (`T=` in FloorSwitch) are not. The Studio uses these where no
capture has seen the sprite (build/centaur/sprite_palettes.json wins).
"""
import collections
import functools
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
LOAD = {"sub_082151E4", "AnimData_Load"}
SELECT = {"sub_08215284", "AnimData_SelectPaletteBank"}


@functools.lru_cache(None)
def pools():
    """{hash: {pool index: [functions]}} from constant call arguments."""
    func, regs, last = None, {}, None
    out = collections.defaultdict(lambda: collections.defaultdict(list))
    for ln in open(os.path.join(ROOT, "gen", "code.s")):
        m = re.match(r"\s*thumb_func_start (\S+)", ln)
        if m:
            func, regs, last = m.group(1), {}, None
            continue
        m = re.match(r"\s*ldr (r\d), \S+ @ =0x([0-9A-F]+)", ln)
        if m:
            regs[m.group(1)] = int(m.group(2), 16)
            continue
        m = re.match(r"\s*movs (r\d), #(0x[0-9a-f]+|\d+)$", ln)
        if m:
            regs[m.group(1)] = int(m.group(2), 0)
            continue
        m = re.match(r"\s*lsls (r\d), (r\d), #(0x[0-9a-f]+|\d+)", ln)
        if m:
            v = regs.get(m.group(2))
            regs[m.group(1)] = (v << int(m.group(3), 0)) & 0xFFFFFFFF if v is not None else None
            continue
        m = re.match(r"\s*adds (r\d), #(0x[0-9a-f]+|\d+)$", ln)
        if m:
            v = regs.get(m.group(1))
            regs[m.group(1)] = v + int(m.group(2), 0) if v is not None else None
            continue
        m = re.match(r"\s*bl (\S+)", ln)
        if m:
            t, r1 = m.group(1), regs.get("r1")
            if t in LOAD:
                last = r1
            elif t in SELECT and last is not None and r1 is not None and r1 < 0x1000:
                if func not in out[last][r1]:
                    out[last][r1].append(func)
            for r in ("r0", "r1", "r2", "r3"):
                regs.pop(r, None)
            continue
        m = re.match(r"\s*(?:adds|subs|mov|ldr|ldrh|ldrb|lsrs|asrs|ands|orrs|eors|muls|negs|rsbs|add|sub) (r\d)\b", ln)
        if m:
            regs.pop(m.group(1), None)
    return {h: dict(v) for h, v in out.items()}


def learned():
    """{"big:HASH": {"pool", "pools", "source"}} in the Studio's learned-palette form."""
    return {f"big:{h:04X}": {"pool": min(v), "pools": sorted(v), "source": "code"} for h, v in pools().items()}


def main():
    want = {int(a, 16) for a in sys.argv[1:]}
    for h, v in sorted(pools().items()):
        if not want or h in want:
            print(f"{h:04X}  " + "  ".join(f"{p} ({', '.join(fs)})" for p, fs in sorted(v.items())))


if __name__ == "__main__":
    main()
