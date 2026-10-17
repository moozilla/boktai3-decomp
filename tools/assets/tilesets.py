#!/usr/bin/env python3
"""Background tiles: the ptn tileset library (CEEF) split into its tilesets,
and the maps that carry their own tiles, each with who uses it.

  tilesets.py list              id, tiles, new tiles, users per tileset
  tilesets.py show ID           one tileset
  tilesets.py json [OUT]        everything (the Studio's /api/tilesets)

ptn layout (docs/ASSETS.md): u16 count, then u32 table, u32 index lists,
u32 tile pool at +0x0C. A table record is u16 id, u16 tiles, u32 offset into
the index lists; an index selects a 4bpp tile in the pool. The pool was
built tileset by tileset in table order: each tileset adds the tiles no
earlier one had as one contiguous run after the previous run (4 add none),
so the library splits cleanly into per-tileset index lists and tile runs.

Users come from the scripts' stage loads, proc_884(map, tileset, hp) (also
proc_885 and proc_876, which both wrap), and the scenes that load each stage
(stages.py, which also gives each scene's BG palette from proc_888). A
tile's palette bank is the one the stage maps' metatiles draw it with.
Output is ROM-derived and stays under build/.
"""
import collections
import csv
import json
import os
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.dirname(HERE))
import archive  # noqa: E402
import lz77  # noqa: E402
from romlib import ROOT, load_rom  # noqa: E402

ROM_BASE = 0x08000000
CACHE = os.path.join(ROOT, "build", "cache", "tilesets.json")


def script_stages():
    """Stage loads in the scripts: [{map, tileset, hp, procs}] for every distinct
    proc_884(map, tileset, hp) (or proc_885, or the loader both wrap, proc_876)."""
    out = subprocess.run([sys.executable, os.path.join(HERE, "gcx.py"), "dis"],
                         cwd=ROOT, capture_output=True, text=True).stdout
    found, proc = {}, None
    for ln in out.split("\n"):
        if ln.startswith("proc "):
            proc = int(ln.split()[1])
            continue
        f = ln.split()
        if (f and f[0] in ("proc_884", "proc_885", "proc_876") and len(f) >= 4
                and all(x.startswith("0x") for x in f[1:4])):
            key = tuple(int(x, 16) for x in f[1:4])
            found.setdefault(key, []).append(proc)
    return [{"map": f"{m:04X}", "tileset": f"{t:04X}", "hp": f"{h:04X}", "procs": ps}
            for (m, t, h), ps in sorted(found.items())]


def entries(rom, label):
    for lab, _, _, ents in archive.archives(rom):
        if lab == label:
            return ents
    return []


def library(rom):
    """The ptn resource: {addr, size, table, index, pool, sets: [...]}, addresses
    absolute. A set: id, tiles (count), ids (pool indices), index_addr,
    new_first/new_count (the pool run it added)."""
    _, addr, size = entries(rom, "ptn")[0]
    p = addr - ROM_BASE
    n, = struct.unpack_from("<H", rom, p)
    T, I, D = struct.unpack_from("<III", rom, p + 0xC)
    sets, hi = [], -1
    for i in range(n):
        sid, k, o = struct.unpack_from("<HHI", rom, p + T + i * 8)
        ids = list(struct.unpack_from(f"<{k}H", rom, p + I + o))
        top = max(ids) if ids else hi
        sets.append({"id": f"{sid:04X}", "tiles": k, "ids": ids, "index_addr": addr + I + o,
                     "new_first": hi + 1, "new_count": max(0, top - hi)})
        hi = max(hi, top)
    return {"addr": addr, "size": size, "table": addr + T, "index": addr + I, "pool": addr + D,
            "pool_tiles": (size - D) // 32, "sets": sets}


def unpack_map(raw):
    """(the map from "MP" on, offset of "MP" in the stored entry or None when
    the entry is LZ77-compressed)."""
    if raw[:1] == b"\x10":
        b = lz77.decompress(raw, 0, limit=0x800000)[0]
        at = None
    else:
        b, at = raw, 0
    if b[:1] != b"M":
        b = b[4:]  # compressed maps carry a 4-byte prefix
        at = None if at is None else 4
    return b, at


def parse_map(b):
    count, ntiles, nmeta = struct.unpack_from("<HHH", b, 4)
    tab, tiles, defs = struct.unpack_from("<III", b, 0xC)
    return {"layers": count, "tiles": ntiles, "metatiles": nmeta, "tile_data": tiles, "defs": defs}


def tile_banks(b, m, n):
    """Per tile 0..n-1 the palette bank its metatile entries use most ('.' = none)."""
    votes = [collections.Counter() for _ in range(n)]
    for i in range(m["metatiles"] * 4):
        e, = struct.unpack_from("<H", b, m["defs"] + i * 2)
        t = e & 0x3FF
        if t < n:
            votes[t][e >> 12] += 1
    return "".join(f"{v.most_common(1)[0][0]:X}" if v else "." for v in votes)


def merge_banks(strings):
    out = []
    for col in zip(*strings):
        c = collections.Counter(x for x in col if x != ".")
        out.append(c.most_common(1)[0][0] if c else ".")
    return "".join(out)


def labels():
    scenes = {int(r["scene"]): r for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "scenes.csv"), encoding="utf-8"))}
    areas = {int(r["area"]): r for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "areas.csv"), encoding="utf-8"))}
    notes = {}
    for r in csv.DictReader(open(os.path.join(ROOT, "data", "notes.csv"), encoding="utf-8")):
        if r["kind"] == "map" and r["label"]:
            notes[r["key"]] = r["label"]
    captured = {r["map"]: r["palette"] for r in csv.DictReader(open(os.path.join(ROOT, "data", "stage_palettes.csv"), encoding="utf-8"))}
    return scenes, areas, notes, captured


def build():
    import areas as areas_mod
    rom = load_rom()
    lib = library(rom)
    stages = script_stages()
    scene_rows, area_rows, map_notes, captured = labels()
    scenes = areas_mod.load_stages()
    ab = areas_mod.build(scenes)
    scene_area = {int(k): v for k, v in ab["scene_area"].items()}
    area_label = {a["area"]: (area_rows.get(a["area"]) or {}).get("place") or a.get("label") or f"Area {a['area']}" for a in ab["areas"]}
    maps = {f"{h:04X}": (a, s) for h, a, s in entries(rom, "map")}
    parsed = {}

    def mp(h):
        if h not in parsed and h in maps:
            a, s = maps[h]
            b, at = unpack_map(rom[a - ROM_BASE:a - ROM_BASE + s])
            parsed[h] = (b, at, parse_map(b))
        return parsed.get(h)

    by_ts = collections.defaultdict(list)
    for st in stages:
        by_ts[st["tileset"]].append(st)
    scenes_of = collections.defaultdict(list)  # stage key -> scenes
    for s in scenes:
        if s.get("stage"):
            st = s["stage"]
            scenes_of[(st["map"], st["tileset"], st["hp"])].append(s)

    def users(sts):
        """maps, stages and scenes (with area and palette) for a list of stage loads."""
        ms, sc, pals, ars = [], [], collections.Counter(), collections.Counter()
        for st in sts:
            if st["map"] not in ms:
                ms.append(st["map"])
            for s in scenes_of.get((st["map"], st["tileset"], st["hp"]), []):
                r = scene_rows.get(s["scene"], {})
                area = scene_area.get(s["scene"])
                sc.append({"scene": s["scene"], "label": r.get("label", ""), "area": area,
                           "place": area_label.get(area, ""),
                           "palette": s.get("palette"), "stage": f"{st['map']}-{st['tileset']}-{st['hp']}"})
                if s.get("palette"):
                    pals[s["palette"]] += 1
                if area is not None:
                    ars[area] += 1
        src = "script" if pals else None
        if not pals:  # a capture's BG palette RAM matched a palette resource
            for h in ms:
                if captured.get(h):
                    pals[captured[h]] += 1
                    src = "capture"
        return {"maps": [{"map": h, "note": map_notes.get(h, "")} for h in ms], "palette_source": src,
                "stages": [{"key": f"{st['map']}-{st['tileset']}-{st['hp']}", "map": st["map"], "hp": st["hp"],
                            "procs": st["procs"]} for st in sts],
                "scenes": sorted(sc, key=lambda x: x["scene"]),
                "palette": pals.most_common(1)[0][0] if pals else None,
                "palettes": [p for p, _ in pals.most_common()],
                "areas": [{"area": a, "place": area_label.get(a, ""), "scenes": n} for a, n in ars.most_common()]}

    out_sets = []
    for order, s in enumerate(lib["sets"]):
        sts = by_ts.get(s["id"], [])
        banks = []
        for h in dict.fromkeys(st["map"] for st in sts):
            m = mp(h)
            if m and m[2]["tile_data"] == 0:
                banks.append(tile_banks(m[0], m[2], s["tiles"]))
        u = users(sts)
        place = u["areas"][0]["place"] if u["areas"] else ""
        out_sets.append(dict(
            {k: v for k, v in s.items() if k != "ids"}, order=order,
            index_size=s["tiles"] * 2, pool_addr=lib["pool"] + s["new_first"] * 32,
            banks=merge_banks(banks) if banks else "." * s["tiles"],
            title=place.split(" (")[0].split(" / ")[0] if place else (u["maps"][0]["note"] if u["maps"] and u["maps"][0]["note"] else ""),
            **u))
    # no stage names a palette: borrow the nearest tileset's in table order
    # (the table is grouped by area), marked as a guess
    for i, s in enumerate(out_sets):
        if s["palette"]:
            continue
        for d in range(1, len(out_sets)):
            near = [out_sets[j] for j in (i - d, i + d) if 0 <= j < len(out_sets) and out_sets[j]["palette_source"] in ("script", "capture")]
            if near:
                s["palette"], s["palette_source"], s["palette_from"] = near[0]["palette"], "neighbor", near[0]["id"]
                break
    # maps with their own tiles: menu and screen backgrounds, a few stages
    staged = collections.defaultdict(list)
    for st in stages:
        staged[st["map"]].append(st)
    bg = []
    for h, (a, size) in maps.items():
        b, at, m = mp(h)
        if m["tile_data"] == 0:
            continue
        u = users(staged.get(h, []))
        bg.append({"map": h, "addr": a, "size": size, "compressed": at is None, "tiles": m["tiles"],
                   "tile_addr": a + at + m["tile_data"] if at is not None else None,
                   "layers": m["layers"], "note": map_notes.get(h, ""),
                   "banks": tile_banks(b, m, m["tiles"]), **u})
        if not bg[-1]["palette"] and captured.get(h):
            bg[-1].update(palette=captured[h], palettes=[captured[h]], palette_source="capture")
    bg.sort(key=lambda x: x["addr"])
    lib = {k: v for k, v in lib.items() if k != "sets"}
    return {"library": lib, "tilesets": out_sets, "bg": bg}


def load():
    """build(), cached in build/cache/tilesets.json until an input changes."""
    ins = [os.path.join(ROOT, p) for p in ("tools/assets/tilesets.py", "tools/assets/stages.py", "tools/assets/gcx.py",
                                           "data/labels/scenes.csv", "data/labels/areas.csv", "data/notes.csv")]
    ins.append(os.path.join(ROOT, "build", "assets", "stages.json"))
    newest = max(os.path.getmtime(p) for p in ins if os.path.exists(p))
    if os.path.exists(CACHE) and os.path.getmtime(CACHE) >= newest:
        return json.load(open(CACHE, encoding="utf-8"))
    out = build()
    os.makedirs(os.path.dirname(CACHE), exist_ok=True)
    with open(CACHE + ".tmp", "w", encoding="utf-8") as f:
        json.dump(out, f, ensure_ascii=False)
    os.replace(CACHE + ".tmp", CACHE)
    return out


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "list"
    d = load()
    if cmd == "list":
        print(f"{'id':5} {'tiles':>5} {'new':>5} {'maps':>4} {'scenes':>6}  title")
        for s in d["tilesets"]:
            print(f"{s['id']:5} {s['tiles']:5} {s['new_count']:5} {len(s['maps']):4} {len(s['scenes']):6}  {s['title']}")
        print(len(d["tilesets"]), "tilesets;", len(d["bg"]), "maps with their own tiles")
    elif cmd == "show":
        s = next(x for x in d["tilesets"] if x["id"] == sys.argv[2].upper())
        print(json.dumps(s, indent=1, ensure_ascii=False))
    elif cmd == "json":
        s = json.dumps(d, ensure_ascii=False)
        if len(sys.argv) > 2:
            open(sys.argv[2], "w", encoding="utf-8").write(s)
        else:
            print(s)


if __name__ == "__main__":
    main()
