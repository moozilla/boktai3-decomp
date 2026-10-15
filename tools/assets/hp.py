#!/usr/bin/env python3
"""Decode the stage terrain resources (type AE6C, magic "HP").

  hp.py list                   every entry: grid size, origin, boxes, paths
  hp.py show HASH              one entry: the height grid, boxes, paths, nav mesh
  hp.py json HASH [HASH...]    decoded entries as JSON (all when none given)

A stage is loaded by the script call proc_884(map, tileset, hp); Terrain_LoadHP
looks the third argument up with ScriptProc_Start(0xAE6C, hp), and the header's
four offsets (from "HP") go to the terrain map gUnk_030052F4 (struct EngineMap):

  +0  "HP\\0\\0"
  +4  u32 grid      -> Terrain_SetGrid, EngineMap+4 (the collision/height grid)
  +8  u32 boxes     -> Terrain_SetBoxes, EngineMap+0xC (named boxes: doors, areas)
  +C  u32 paths     -> Terrain_SetPaths, EngineMap+0x10 (patrol paths)
  +10 u32 nav       -> Terrain_SetNavMeshes, EngineMap+0x14 (0 = none; a nav mesh)

Grid: u32 0, u16 width, u16 height (cells; Terrain_UpdateSize copies them to
gUnk_030052F8/FC), s16 origin x, s16 origin y (map pixels * 16), then
width * height cells of four bytes (struct SceneCell, cells start at +12 as in
scene_object.c): u8 height (low nibble = level, high nibble = kind: 0 flat, 1
slope), u8 tile, u16 flags (bit 3 blocks entry; 0x0003 outside the stage,
0x040B walls, 0x000A slope ends; meanings tentative).

World coordinates (GameState+0x30/+0x32/+0x34 = x, y, z, s16) use 256 units
per cell and per height level; x and z run along the grid, y is height. The
BG map pixel of a point is

  px = origin_x / 16 + 3 * (x - z) / 32
  py = origin_y / 16 + 3 * (x + z - y) / 32

(fitted to the BG2 camera at 0x03004CB0/CB2 in a replay: the camera keeps the
player at screen (120, 94), residual under one pixel).

Boxes: u32 count, then 12-byte records u16 x0, z0, x1, z1 (world units),
u8 y0, y1 (world units / 16), u16 name hash. Scripts name them: door
triggers (`proc_934 box scene entrance ...`), entrances (`proc_909 box facing
id`), camera areas (`proc_895 box`), traps (`trap box handler`).

Paths: u32 count, count pairs (u16 points, u16 offset from the section), then
8-byte points u16 x, z, u8 wait, u8 direction (256 = full turn), u16 flags.
Enemy spawners take a path index (`proc_1496 name path ...`, tentative).

Nav meshes: u32 count, count u32 offsets (from the section); a mesh is u16
rects, u16 portals, u32 extra, u32 rect, portal and extra offsets (from the
mesh).
Rects are 16 bytes: u8 x0, z0, x1, z1 (cells), 12 portal indices (0xFF = none);
portals 12 bytes: u16 x, z, u16 ?, u8 rect a, u8 rect b, 4 bytes 0xFF; then
`extra` u16 values (unknown). Only the layout is known.

Output is ROM-derived; keep dumps under build/.
"""
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import load_rom  # noqa: E402
import archive  # noqa: E402
import lz77  # noqa: E402

ROM_BASE = 0x08000000
TYPE = 0xAE6C


def entries(rom):
    """{hash: raw bytes} of the HP archive."""
    for label, t, base, ents in archive.archives(rom):
        if t == TYPE:
            return {h: rom[a - ROM_BASE:a - ROM_BASE + s] for h, a, s in ents}
    return {}


def unpack(raw):
    """The decompressed resource from "HP" on (LZ77 data carries a 4-byte prefix)."""
    if raw[:1] == b"\x10":
        raw = lz77.decompress(raw, 0)[0]
    i = raw.find(b"HP\0\0")
    if i < 0 or i > 8:
        raise ValueError("no HP header")
    return raw[i:]


def decode(raw):
    b = unpack(raw)
    grid_o, box_o, path_o, nav_o = struct.unpack_from("<4I", b, 4)
    _, w, h, ox, oy = struct.unpack_from("<IHHhh", b, grid_o)
    cells = []
    for i in range(w * h):
        hgt, tile, flags = struct.unpack_from("<BBH", b, grid_o + 12 + i * 4)
        cells.append([hgt & 15, hgt >> 4, tile, flags])
    out = {"width": w, "height": h, "origin": [ox / 16, oy / 16], "cells": cells, "boxes": [], "paths": [],
           "nav": []}
    n, = struct.unpack_from("<I", b, box_o)
    for i in range(n):
        x0, z0, x1, z1, y0, y1, name = struct.unpack_from("<4HBBH", b, box_o + 4 + i * 12)
        out["boxes"].append({"name": f"{name:04X}", "x0": x0, "z0": z0, "x1": x1, "z1": z1,
                             "y0": y0 * 16, "y1": y1 * 16})
    n, = struct.unpack_from("<I", b, path_o)
    for i in range(n):
        pts, off = struct.unpack_from("<HH", b, path_o + 4 + i * 4)
        path = []
        for k in range(pts):
            x, z, wait, d, flags = struct.unpack_from("<HHBBH", b, path_o + off + k * 8)
            path.append({"x": x, "z": z, "wait": wait, "dir": d, "flags": flags})
        out["paths"].append(path)
    if nav_o:
        n, = struct.unpack_from("<I", b, nav_o)
        for i in range(n):
            o = nav_o + struct.unpack_from("<I", b, nav_o + 4 + i * 4)[0]
            nr, npo, nx, ro, po, xo = struct.unpack_from("<HHIIII", b, o)
            rects = []
            for k in range(nr):
                r = b[o + ro + k * 16:o + ro + k * 16 + 16]
                rects.append({"x0": r[0], "z0": r[1], "x1": r[2], "z1": r[3],
                              "portals": [v for v in r[4:] if v != 0xFF]})
            portals = []
            for k in range(npo):
                x, z, u, ra, rb = struct.unpack_from("<HHHBB", b, o + po + k * 12)
                portals.append({"x": x, "z": z, "u": u, "rects": [ra, rb]})
            extra = list(struct.unpack_from(f"<{nx}H", b, o + xo))
            out["nav"].append({"rects": rects, "portals": portals, "extra": extra})
    return out


def project(hp, x, y, z):
    """BG map pixel of a world point (see the module docstring)."""
    return hp["origin"][0] + 3 * (x - z) / 32, hp["origin"][1] + 3 * (x + z - y) / 32


def main():
    rom = load_rom()
    ents = entries(rom)
    cmd = sys.argv[1] if len(sys.argv) > 1 else "list"
    if cmd == "list":
        bad = 0
        for h in sorted(ents):
            try:
                d = decode(ents[h])
            except (ValueError, struct.error, TypeError) as e:
                bad += 1
                print(f"{h:04X} ? {e}")
                continue
            print(f"{h:04X} {d['width']:3}x{d['height']:<3} origin {d['origin'][0]:5g},{d['origin'][1]:<5g} "
                  f"boxes {len(d['boxes']):3} paths {len(d['paths']):2} nav {len(d['nav'])} "
                  + " ".join(x["name"] for x in d["boxes"][:8]))
        print(f"{len(ents)} entries, {bad} not decoded")
    elif cmd == "show":
        h = int(sys.argv[2], 16)
        d = decode(ents[h])
        print(f"{h:04X}: {d['width']}x{d['height']} cells, origin {d['origin']} (map pixels)")
        print("height grid (level; s = slope, # = blocks, . = outside):")
        for z in range(d["height"]):
            row = []
            for x in range(d["width"]):
                lvl, kind, tile, flags = d["cells"][z * d["width"] + x]
                row.append("  ." if flags == 3 and lvl == 0 else f"{lvl:2}{'s' if kind else '#' if flags & 8 else ' '}")
            print(" ".join(row))
        for x in d["boxes"]:
            print(f"box {x['name']}: x {x['x0']}-{x['x1']} z {x['z0']}-{x['z1']} y {x['y0']}-{x['y1']}")
        for i, p in enumerate(d["paths"]):
            print(f"path {i}: " + " ".join(f"({q['x']},{q['z']})" for q in p))
        for m in d["nav"]:
            print(f"nav: {len(m['rects'])} rects, {len(m['portals'])} portals, {len(m['extra'])} extra")
    elif cmd == "json":
        want = [int(x, 16) for x in sys.argv[2:]] or sorted(ents)
        json.dump({f"{h:04X}": decode(ents[h]) for h in want}, sys.stdout)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
