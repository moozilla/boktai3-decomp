"""Cartridge ABI. One bank per room, with signed BG tiles and unsigned OBJ tiles."""
import re
import struct

import numpy as np
from PIL import Image

from art import ROOT, actor_pose, encode_tile, fit_tiles, pose_tiles, unpack_4bpp
from rooms import nearest_floor, room_events, walkable
import text as game_text

# Bank addresses stay fixed so the same SM83 code can run every room.
LAYOUT = dict(header=0x4000, route=0x4020, events=0x4100, rows=0x4200, projection=0x4300,
              cells=0x4400, patterns=0x4800, background=0x5800, window=0x5C00, worldmap=0x6000, projection_hi=0x7000, skeleton_routes=0x7100)
SCENES = [2698, 2705, 2712]
ROOM_NAMES = {2698: "SEALED PRISON", 2705: "THE CORRIDOR", 2712: "THE SKYLIGHT"}
STORY = {2698: 5599, 2705: 8697, 2712: 8701}


def icon(rows):
    return np.array([[int(c) for c in line] for line in rows.split()], dtype=np.uint8)


SUN = icon("33303333 30333033 33300333 03000030 33000033 33300333 30333033 33303333")
FULL = icon("33333333 32222223 31000013 31000013 31000013 31000013 32222223 33333333")
EMPTY = icon("33333333 32222223 32333323 32333323 32333323 32333323 32222223 33333333")
HEART = icon("33333333 30033003 00000000 00000000 30000003 33000033 33300333 33333333")
HOLLOW = icon("33333333 32233223 23322332 23333332 32333323 33233233 33322333 33333333")


class Window:
    def __init__(self, source):
        self.tiles = []
        self.font = source.entry("fnt")
        self.map = np.zeros((32, 32), np.uint8)
        self.tile(np.full((8, 8), 3, np.uint8))
        self.ids = [self.tile(t) for t in (FULL, EMPTY, HEART, HOLLOW)]
        self.map[0, 1] = self.tile(SUN)
        self.map[0, 2:8] = self.ids[1]
        self.map[0, 10:13] = self.ids[2]

    def tile(self, tile):
        data = encode_tile(tile)
        if data not in self.tiles:
            self.tiles.append(data)
        return self.tiles.index(data)

    def ascii(self, x, y, line):
        for col, char in enumerate(line, x):
            pos = 12 + (ord(char) - 32) * 64
            glyph = unpack_4bpp(self.font[pos:pos + 64]).reshape(16, 8)[1:15]
            # Retain the darkest stroke from each pair of source rows.
            ink = np.full((8, 8), 3, np.uint8)
            ink[:7] = np.minimum(3, glyph.reshape(7, 2, 8).min(axis=1) - 1)
            self.map[y, col] = self.tile(ink)

    def dialogue(self, source, scene):
        # A short original line, in its own font. Keep the actual raw glyph IDs,
        # including identical-looking characters mapped to different slots.
        n = STORY[scene]
        offset, = struct.unpack_from("<I", source.rom, game_text.TABLE - 0x08000000 + 4 * n)
        a = game_text.BASE - 0x08000000 + (offset & 0x7FFFFFFF)
        raw = source.rom[a:source.rom.index(b'\0', a)]
        raw = re.sub(rb"<LABEL=[^>]*>.*?</LABEL>", b"", raw, flags=re.S)
        raw = re.sub(rb"<[^>]*>", b"", raw)
        codes = []
        i = 0
        while i < len(raw) and len(codes) < 9:
            if 0x80 <= raw[i] <= 0x85:
                codes.append((raw[i] - 128) * 256 + raw[i + 1])
                i += 2
            else:
                i += 1
        for k, code in enumerate(codes):
            addr = 0x200C + code * 128
            glyph = np.minimum(3, unpack_4bpp(self.font[addr:addr + 128]) - 1)
            for t, tile in enumerate(glyph):
                self.map[4 + t // 2, 1 + 2 * k + t % 2] = self.tile(tile)
        return n

    def populate(self, source, scene, bank):
        self.ascii(16, 0, f"{bank:02d}")
        self.ascii(1, 2, ROOM_NAMES.get(scene, f"SCENE {scene}"))
        if scene in STORY:
            self.dialogue(source, scene)
        self.ascii(1, 7, "A  FIRE / DOOR")
        self.ascii(1, 8, "B  STORE SUNLIGHT")
        self.ascii(1, 10, "START  RETURN")


def pack_room(source, room, rooms):
    scene = room.stage["scene"]
    bank = rooms[scene][1]
    ui = Window(source)
    ui.populate(source, scene, bank)
    patterns, tilemap, display, unique = fit_tiles(room.background, 256 - len(ui.tiles))
    # Signed BG indices 0x80..0xff,0..0x7f address sequential $8800..$97ff.
    count = len(patterns)
    encode_id = lambda i: (i + count + 128) & 255
    all_tiles = b"".join(encode_tile(t) for t in patterns) + b"".join(ui.tiles)
    bgmap = np.bitwise_xor(tilemap, 128).tobytes()
    winmap = ((ui.map.astype(np.uint16) + count + 128) & 255).astype(np.uint8).tobytes()
    t = room.terrain
    w, h = t["width"], t["height"]
    if max(w, h) > 32:
        raise ValueError(f"{scene}: playable ABI supports HP grids up to 32x32; use the room exporter")
    entry = next((e for e in room.stage["entrances"] if e.get("x") is not None), None)
    if entry:
        start = nearest_floor(room, entry["x"], entry["z"])
    else:
        start = nearest_floor(room, 0, 0)
    enemy = next((e for e in room.stage["enemies"] if "ghoul" in e["families"] and e.get("route")), None)
    path = t["paths"][enemy["path"]] if enemy else []
    skeletons = []
    for e in room.stage["enemies"]:
        if "skeleton" in e["families"] and e.get("route") and e["path"] not in skeletons:
            skeletons.append(e["path"])
    skeletons = skeletons[:2] if not path else []
    sk_routes = bytearray()
    for p in skeletons:
        points = t["paths"][p][:31]
        route_bytes = bytes(v for q in points for v in (*nearest_floor(room, q["x"], q["z"]), q["wait"], q["dir"]))
        sk_routes.extend((bytes([len(points)]) + route_bytes).ljust(128, b"\0"))
    events = room_events(room, rooms)
    route = bytes(v for p in path for v in (*nearest_floor(room, p["x"], p["z"]), p["wait"], p["dir"]))
    ox, oy = t["origin"]
    offset = [round((ox - room.left) * room.scale) & 255, round((oy - room.top) * room.scale) & 255]
    header = bytes([*offset, w, h, *start, len(path), len(events), *(encode_id(i) for i in ui.ids)])
    header += struct.pack("<hh", round((ox - room.left) * room.scale), round((oy - room.top) * room.scale))
    max_on_line = 3 * (1 + bool(path) + len(skeletons)) + sum(e[0] in (2, 3) for e in events) + 1
    if 6 * (1 + bool(path) + len(skeletons)) + len(events) + 1 > 40:
        raise ValueError(f"scene {scene} exceeds the OAM table")
    header += bytes([len(skeletons), max_on_line > 10])
    sections = {
        "header": header, "route": route, "events": bytes(v for e in events for v in e),
        "rows": b"".join(struct.pack("<H", LAYOUT["cells"] + row * w) for row in range(h)),
        "projection": bytes(round(i * 3 * room.scale) & 255 for i in range(256)),
        "cells": bytes(c[0] | c[1] << 4 if walkable(c) else 255 for c in t["cells"]),
        "patterns": all_tiles, "background": bytes(1024), "window": winmap, "worldmap": bgmap,
        "skeleton_routes": sk_routes,
        "projection_hi": bytes(round(i * 3 * room.scale) >> 8 for i in range(256)),
    }
    buf = bytearray(0x3200)
    for (name, addr), end in zip(LAYOUT.items(), [*list(LAYOUT.values())[1:], 0x7200]):
        data = sections[name]
        if len(data) > end - addr:
            raise ValueError(f"scene {scene}: {name} exceeds its bank region")
        buf[addr - 0x4000:addr - 0x4000 + len(data)] = data
    info = dict(scene=scene, resources=room.stage["stage"], palette=room.palette, scale=room.scale,
                crop=[room.left, room.top], start=start, route=[list(p.values()) for p in path], events=events,
                skeleton_paths=skeletons, source_tiles=unique, bg_tiles=count, window_tiles=len(ui.tiles), text_id=STORY.get(scene))
    return bytes(buf), info, display


def ghoul_palette(source):
    ghoul_code = (ROOT / "src/ghoul.c").read_text()
    base = int(re.search(r"MonsterInit_ReadPalette\(actor, (0x[0-9A-Fa-f]+)", ghoul_code)[1], 16)
    spawn = next(t for _, t in source.walker.text(1496) if t.startswith("command 0x5a5 "))
    variant = int(re.search(r" C: (\d+)", spawn)[1])
    return base + variant


def objects(source):
    _, django = source.actor(0xDE23)
    _, ghoul = source.actor(0x9247, ghoul_palette(source))
    # Five direction families. The mirror bit provides the other three.
    # Take a key from each of the first five walking sequences in DE23.
    import sprites
    pkg = sprites.Package(source.entry("tCB05", 0xDE23))
    anim = list(pkg.animations())
    poses = []
    for i in range(5):
        for seq, key in ((i, 0), (5 + i, 2)):
            frame, _ = anim[seq]["keys"][key]
            poses.append(pose_tiles(actor_pose(django(frame), size=(24, 32), scale=.85)))
    for frame in (0, 2, 5, 7):
        poses.append(pose_tiles(actor_pose(ghoul(frame), size=(24, 32), scale=.85)))
    _, skeleton = source.actor(0xAE52)
    for frame in (0, 3, 5, 7):
        poses.append(pose_tiles(actor_pose(skeleton(frame), size=(24, 32), scale=.85)))
    # Custom interface sprites for dropped fruit, skylight and solar shot.
    markers = [
        "00000000 00011000 00013000 00333300 03111130 03112130 00333300 00000000",
        "00010000 01010100 00333000 01111100 03111310 00333000 01010100 00010000",
        "00000000 00001000 00013100 00111110 00013100 00001000 00000000 00000000",
    ]
    marker_tiles = bytearray()
    for rows in markers:
        tile = np.zeros((16, 8), np.uint8)
        tile[-8:] = icon(rows)
        marker_tiles.extend(pose_tiles(tile))
    return poses, bytes(marker_tiles)


def enemy_constants():
    # Lower values at their decision sites. A changed C expression must be
    # reviewed before the generated state machine silently acquires new timing.
    states = (ROOT / "src/ghoul_states.c").read_text()
    chase = (ROOT / "src/ghoul.c").read_text()
    def literal(source, function, pattern):
        body = re.search(r"^void " + function + r"\([^;]*?\)\s*\{(.*?)^}", source, re.M | re.S)[1]
        return int(re.search(pattern, body)[1], 0)
    skeleton = (ROOT / "src/skeleton_main.c").read_text()
    return {
        "lunge_speed": literal(skeleton, "Skeleton_TickDecayingLunge", r"context\[0x6D1\] = (0x[0-9A-Fa-f]+)"),
        "lunge_period": literal(skeleton, "Skeleton_TickDecayingLunge", r"Mod\(tick, (\d+)\)"),
        "death_mark": literal(states, "Ghoul_UpdateDeathAnimation", r"frame == (\w+)"),
        "death_end": literal(states, "Ghoul_UpdateDeathAnimation", r"frame > (\w+)") + 1,
        "delay_end": literal(chase, "Ghoul_ResumeDelayedChase", r"counter2AC <= (\w+)") + 1,
    }
