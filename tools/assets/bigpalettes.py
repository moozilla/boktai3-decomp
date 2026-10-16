#!/usr/bin/env python3
"""The sprite palette pool entries (9B1B) each large sprite is drawn with.

  bigpalettes.py [HASH...]     print hash, default, variants and their sources
  bigpalettes.py write         regenerate data/sprite_palettes.csv

Every large sprite has a default entry in its own data: the u16 at +2 of each
frame record (the same in every frame), which AnimData_Load applies. Code
then selects variants for different monsters or states sharing one sprite
(the ghoul 9247 defaults to 400 and is drawn with 403 and 404). Variants come
from two sources: a runtime log of every AnimData_SelectPaletteBank call while
the game visits every scene (tools/mgba/palette_survey.lua ->
build/centaur/sprite_pools.tsv, folded into data/sprite_palettes.csv by
`write`), and constant call pairs in the code (below).

Code pairs: AnimData_Load(anim, hash) (082151E4) and then
AnimData_SelectPaletteBank(anim, pool_index) (08215284), which points the
sprite's bank 0 at pool entry `pool_index` (index < *0x030042E0, the entry at
*0x030042E4 + 32 * index). This scans gen/code.s for those calls with
constant arguments, pairing each palette call with the latest load in the
same function. One sprite can take several entries: nine enemy routines draw
7E7D, each in its own palette (439-453), and 7E82 is the statue (549) and the
armor (551).

Only constant hashes are found (35 sprites); sprites chosen from tables or
script options (`T=` in FloorSwitch) are covered by the runtime log only.
Variants that neither source has seen (attacks, bosses and menus the survey
does not reach) are still missing; the default is always known.
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


CSV = os.path.join(ROOT, "data", "sprite_palettes.csv")
RUNTIME = os.path.join(ROOT, "build", "centaur", "sprite_pools.tsv")


@functools.lru_cache(None)
def defaults():
    """{hash: default pool entry} from the sprite data (frame +2)."""
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    sys.path.insert(0, os.path.join(ROOT, "tools"))
    from bigsprites import BigSprites
    from romlib import load_rom
    bs = BigSprites(load_rom())
    out = {}
    for i, r in enumerate(bs.records):
        fr = next(iter(bs.frames(i)), None)
        if fr:
            out[r[0]] = fr[2]
    return out


def runtime():
    """{hash: {pool: set of callers}} from the palette survey log, if present."""
    import csv
    out = collections.defaultdict(lambda: collections.defaultdict(set))
    if os.path.exists(RUNTIME):
        for r in csv.DictReader(open(RUNTIME), delimiter="\t"):
            if r["hash"] != "?":
                out[int(r["hash"], 16)][int(r["pool"])].add(r["caller"])
    return out


@functools.lru_cache(None)
def table():
    """{hash: {"default", "variants": {pool: sources}}} from data/sprite_palettes.csv."""
    import csv
    out = {}
    if os.path.exists(CSV):
        for r in csv.DictReader(open(CSV, encoding="utf-8")):
            v = {}
            for part in filter(None, r["variants"].split(";")):
                p, _, src = part.partition("@")
                v[int(p)] = src
            out[int(r["hash"], 16)] = {"default": int(r["default"]), "variants": v}
    return out


def variants(h):
    """Sorted pool entries the game draws sprite h with (default first)."""
    t = table().get(h, {})
    vs = set(t.get("variants", {})) | set(pools().get(h, {}))
    d = t.get("default", defaults().get(h))
    return [d] + sorted(vs - {d}) if d is not None else sorted(vs)


def write():
    import csv
    rt, code, d = runtime(), pools(), defaults()
    old = table()
    with open(CSV, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["hash", "default", "variants"])
        for h in sorted(d):
            v = {p: s for p, s in old.get(h, {}).get("variants", {}).items()}
            for p, callers in rt.get(h, {}).items():
                v[p] = "run " + " ".join(sorted(callers))
            for p, fs in code.get(h, {}).items():
                v.setdefault(p, "code " + " ".join(fs))
            w.writerow([f"{h:04X}", d[h], ";".join(f"{p}@{v[p]}" for p in sorted(v))])
    print(f"{CSV}: {len(d)} sprites, {sum(1 for h in d if h in rt or h in code)} with variants seen")


def main():
    if sys.argv[1:] == ["write"]:
        return write()
    want = {int(a, 16) for a in sys.argv[1:]}
    t = table()
    for h in sorted(defaults()):
        if not want or h in want:
            vs = t.get(h, {}).get("variants", {})
            print(f"{h:04X}  default {defaults()[h]}  " + "  ".join(f"{p} ({vs.get(p) or 'code ' + ' '.join(pools().get(h, {}).get(p, []))})" for p in variants(h)[1:]))


if __name__ == "__main__":
    main()
