"""Lower a decoded stage to DMG background, terrain and room events."""
from dataclasses import dataclass
import math
import re
import struct

import numpy as np
from PIL import Image, ImageDraw

from art import ROOT, encode_tile, fit_tiles, hp, quantize_bg, room_ramp, screen_image, unpack_4bpp


@dataclass
class Room:
    stage: dict
    terrain: dict
    color: Image.Image
    background: np.ndarray
    palette: str
    scale: float
    left: int
    top: int

    def project(self, x, z):
        cell = self.terrain["cells"][(z // 256) * self.terrain["width"] + x // 256]
        level, shape = cell[:2]
        y = level * 256 - ((z & 255) if shape == 1 else (x & 255) if shape == 2 else 0)
        px, py = hp.project(self.terrain, x, y, z)
        return round((px - self.left) * self.scale), round((py - self.top) * self.scale)


def walkable(cell):
    # SceneActorTerrain_CanTraverseCell (scene_actor_utilities.c).
    return not (cell[3] & 0x43)


def convert(source, number, palette=None):
    stage, terrain = source.scene(number)
    mp = stage["stage"]
    indexed = source.map(mp["map"], mp["tileset"])
    key, colors = source.palette(stage, palette)
    # Fit the walkable footprint plus a margin for walls, leaving other
    # layers outside the viewport. Snap 1/2-scale rooms to source tile edges.
    points = []
    for i, cell in enumerate(terrain["cells"]):
        if walkable(cell):
            x, z = (i % terrain["width"]) * 256, (i // terrain["width"]) * 256
            for dx, dz in ((0, 0), (255, 0), (0, 255), (255, 255)):
                y = cell[0] * 256 - (dz if cell[1] == 1 else dx if cell[1] == 2 else 0)
                points.append(hp.project(terrain, x + dx, y, z + dz))
    if not points:
        raise ValueError(f"scene {number} contains no walkable terrain")
    x, y = np.array(points).T
    extent = max(np.ptp(x) + 48, np.ptp(y) + 64)
    # Prefer simple sample ratios to keep repeated masonry tiles in phase.
    scale = next((s for s in (1, .75, .5, .375, .25, .1875, .125) if extent * s <= 256),
                 256 / extent)
    side = round(256 / scale)
    left = round(((x.min() + x.max() - side) / 2) / 8) * 8
    top = round(((y.min() + y.max() - side) / 2 - 12) / 8) * 8
    box = (left, top, left + side, top + side)
    color = Image.fromarray(colors[indexed]).crop(box).resize((256, 256), Image.Resampling.LANCZOS)
    lookup = np.uint8(np.rint(room_ramp(indexed, colors) * 255))
    plane = Image.fromarray(lookup[indexed]).crop(box).resize((256, 256), Image.Resampling.LANCZOS)
    return Room(stage, terrain, color, quantize_bg(plane), key, scale, left, top)


def nearest_floor(room, x, z):
    """Snap a door center in a blocked wall cell to the nearest HP floor sample."""
    t = room.terrain
    x, z = x // 32, z // 32
    def valid(a, b):
        return 0 <= a < t["width"] * 8 and 0 <= b < t["height"] * 8 and walkable(
            t["cells"][(b // 8) * t["width"] + a // 8])
    if valid(x, z):
        return x, z
    return min(((a, b) for a in range(t["width"] * 8) for b in range(t["height"] * 8) if valid(a, b)),
               key=lambda p: (p[0] - x) ** 2 + (p[1] - z) ** 2)


def room_events(room, rooms):
    """Choose the first opening-story branch at duplicate door and item sites."""
    events = []
    for d in room.stage["doors"]:
        n = d.get("scene")
        if n not in rooms or "rect" not in d:
            continue
        dest, bank = rooms[n]
        entrance = next((e for e in dest.stage["entrances"] if e["id"] == d["entrance"]), None)
        if not entrance or entrance.get("x") is None:
            continue
        x0, z0, x1, z1 = d["rect"]
        # A at the threshold can activate doors whose center is in a wall.
        events.append((1, max(0, x0 // 32 - 3), max(0, z0 // 32 - 3), x1 // 32 + 2,
                       z1 // 32 + 2, bank, *nearest_floor(dest, entrance["x"], entrance["z"])))
    items = set()
    for item in room.stage["items"]:
        if item["category"] != 0 or item["item"] not in (1, 13) or item.get("x") is None:
            continue
        p = nearest_floor(room, item["x"], item["z"])
        if p in items:
            continue
        items.add(p)
        x, z = p
        events.append((2, x - 3, z - 3, x + 3, z + 3, len(items) - 1, x, z))
    if room.stage["scene"] == 2712:
        # proc_2716 -> proc_1072(..., x, z, ...). Verify the position from
        # the script during the build, including its cell-center adjustment.
        x, z = room.stage["skylight"]
        events.append((3, x - 5, z - 5, x + 5, z + 5, 0, x, z))
    return list(dict.fromkeys(events))


def place_skylight(source, room):
    if room.stage["scene"] == 2712:
        args = next(t.split() for _, t in source.walker.text(2716) if t.startswith("proc_1072 "))
        room.stage["skylight"] = nearest_floor(room, int(args[2], 0) + 128, int(args[3], 0) + 128)


def terrain_overlay(room):
    im = room.color.copy()
    dr = ImageDraw.Draw(im)
    for i, cell in enumerate(room.terrain["cells"]):
        if walkable(cell):
            x, z = (i % room.terrain["width"]) * 256, (i // room.terrain["width"]) * 256
            polygon = [room.project(x + dx, z + dz) for dx, dz in ((0, 0), (255, 0), (255, 255), (0, 255))]
            dr.line(polygon + polygon[:1], fill=(253, 219, 84) if cell[1] else (62, 222, 169))
    return im
