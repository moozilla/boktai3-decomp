"""ROM assets to original Game Boy tiles. All intermediate images are generated."""
import csv
from functools import lru_cache
import struct
import sys
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT / "tools"), str(ROOT / "tools/assets")]
import archive
import bigsprites
import gfx
import hp
import romlib
import sprites
import stages
import tilesets

# Physical index zero is transparent for OBJ. BGP, OBP0, OBP1 map the
# remaining indices to 0..3, lightest to darkest, on a monochrome LCD.
BGP, OBP0, OBP1 = 0xE4, 0xD0, 0xE0
SHADES = np.array([255, 170, 85, 0], dtype=np.uint8)
BAYER = (np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]) + .5) / 16 - .5


def lightness(rgb):
    """CIE L* from sRGB, normalized to a unit interval."""
    c = np.asarray(rgb, dtype=float) / 255
    c = np.where(c < .04045, c / 12.92, ((c + .055) / 1.055) ** 2.4)
    y = c @ np.array([.2126, .7152, .0722])
    return np.where(y > .008856, 1.16 * np.cbrt(y) - .16, 9.033 * y)


def unpack_4bpp(raw):
    b = np.frombuffer(raw, np.uint8).reshape(-1, 8, 4)
    return np.stack((b & 15, b >> 4), axis=-1).reshape(-1, 8, 8)


def encode_tile(tile):
    tile = np.asarray(tile, dtype=np.uint8)
    if tile.shape != (8, 8) or np.any(tile > 3):
        raise ValueError("expected an 8 by 8 four-color tile")
    return np.packbits(np.stack((tile & 1, tile >> 1), axis=1), axis=-1).tobytes()


def screen_image(index):
    return Image.fromarray(SHADES[index])


class Source:
    def __init__(self):
        self.rom = romlib.load_rom()
        self.archives = {label: {h: (romlib.off(a), size) for h, a, size in ents}
                         for label, _, _, ents in archive.archives(self.rom)}
        self.ptn = tilesets.library(self.rom)
        self.tile_sets = {s["id"]: s["ids"] for s in self.ptn["sets"]}
        self.big = bigsprites.BigSprites(self.rom)
        self.big_ids = {r[0]: i for i, r in enumerate(self.big.records)}
        self.pool = self.entry("t9B1B")
        self.walker = None

    def entry(self, archive_name, key=None):
        entries = self.archives[archive_name]
        offset, size = entries[next(iter(entries)) if key is None else key]
        return self.rom[offset:offset + size]

    @lru_cache(maxsize=128)
    def scene(self, number):
        if self.walker is None:
            self.walker = stages.Walker(self.rom)
        scene = self.walker.scene(number)
        if not scene["stage"]:
            raise ValueError(f"scene {number} has no decoded stage load")
        key = int(scene["stage"]["hp"], 0)
        terrain = hp.decode(self.entry("tAE6C", key))
        calls = scene["calls"]
        scene["skylights"] = [(int(args[1], 0) + 128, int(args[2], 0) + 128)
                              for c in calls if c["proc"] == 1072 and len(args := c["args"]) >= 3
                              and all(stages.num(v) is not None for v in args[1:3])]
        scene["songs"] = [stages.num(c["args"][0]) for c in calls if c["proc"] == 1048 and c["args"]
                          and stages.num(c["args"][0]) is not None]
        stages.resolve(scene, {key: terrain}, {})
        return scene, terrain

    def map(self, map_id, tileset_id):
        """Composite palette indices in layer order; index 0 of each bank is transparent."""
        raw, _ = tilesets.unpack_map(self.entry("map", int(map_id, 16)))
        nlayer, ntiles, nmeta, table, own, defs = struct.unpack_from("<3H2x3I", raw, 4)
        if own:
            tile = unpack_4bpp(raw[own:own + ntiles * 32])
        else:
            ids = self.tile_sets[tileset_id]
            pool = romlib.off(self.ptn["pool"])
            tile = unpack_4bpp(b"".join(self.rom[pool + i * 32:pool + (i + 1) * 32] for i in ids))
        halves = np.frombuffer(raw, "<u2", nmeta * 4, defs)
        ids = halves & 1023
        chars = tile[np.minimum(ids, len(tile) - 1)].copy()
        chars[ids >= len(tile)] = 0
        flip = (halves & 0x400) != 0
        chars[flip] = chars[flip, :, ::-1]
        flip = (halves & 0x800) != 0
        chars[flip] = chars[flip, ::-1, :]
        chars = np.where(chars, chars + ((halves >> 12) * 16)[:, None, None], 0).astype(np.uint8)
        meta = chars.reshape(-1, 2, 2, 8, 8).transpose(0, 1, 3, 2, 4).reshape(-1, 16, 16)
        meta = np.concatenate((meta, np.zeros((1, 16, 16), dtype=np.uint8)))
        layers = [struct.unpack_from("<HHII", raw, table + 12 * i) for i in range(nlayer)]
        out = np.zeros((max(l[1] for l in layers) * 16, max(l[0] for l in layers) * 16), np.uint8)
        for w, h, _, off in reversed(layers):
            cells = np.frombuffer(raw, "<u2", w * h, off)
            if w % 16 == 0 and h % 16 == 0:
                cells = cells.reshape(h // 16, w // 16, 16, 16).transpose(0, 2, 1, 3)
            pixels = meta[np.minimum(cells.reshape(h, w), nmeta)].transpose(0, 2, 1, 3).reshape(h * 16, w * 16)
            region = out[:h * 16, :w * 16]
            np.copyto(region, pixels, where=pixels != 0)
        return out

    def palette(self, scene, override=None):
        key = override or scene.get("palette")
        if not key:
            with (ROOT / "data/stage_palettes.csv").open() as f:
                key = next((r["palette"] for r in csv.DictReader(f) if r["map"] == scene["stage"]["map"]), None)
        if not key:
            raise ValueError("no evidenced BG palette; supply --palette HASH")
        return key, np.array(gfx.read_pal(self.entry("t92B3", int(key, 16)), 20, 256), dtype=np.uint8)

    def actor(self, key, override=None):
        if key in self.archives["tCB05"]:
            pkg = sprites.Package(self.entry("tCB05", key))
            base = pkg.h0 if override is None else override
            pool_count = struct.unpack_from("<H", self.pool)[0]
            banks = [gfx.read_pal(self.pool, 4 + 32 * (base + i)) if base + i < pool_count
                     else [(0, 0, 0)] * 16 for i in range(16)]
            frames = list(pkg.frames())
            return len(frames), lambda i: pkg.render(frames[i], banks)
        index = self.big_ids[key]
        frames = list(self.big.frames(index))

        def render(i):
            bank = frames[i][2] if override is None else override
            return self.big.render(frames[i], gfx.read_pal(self.pool, 4 + 32 * bank))
        return len(frames), render


def room_ramp(indices, palette):
    """Expand each used sub-palette gently while preserving the scene's lighting."""
    light = lightness(palette)
    counts = np.bincount(indices.ravel(), minlength=256)
    used = (counts > 0) & (np.arange(256) % 16 != 0)
    if not np.any(used):
        return light
    lo, hi = np.percentile(light[used], [3, 97])
    overall = np.clip((light - lo) / max(.25, hi - lo), 0, 1)
    ramp = overall.copy()
    for base in range(0, 256, 16):
        bank = slice(base, base + 16)
        levels = light[bank][used[bank]]
        if len(levels) >= 3:
            local = np.clip((light[bank] - levels.min()) / max(.25, np.ptp(levels)), 0, 1)
            ramp[bank] = .76 * overall[bank] + .24 * local
    ramp[0] = overall[0]
    border = np.concatenate((indices[0], indices[-1], indices[:, 0], indices[:, -1]))
    backdrop = np.bincount(border, minlength=256).argmax()
    # Reserve the dark surround. Lift shadow detail only in low-key rooms.
    same_color = np.all(palette == palette[backdrop], axis=1)
    active = (counts > 0) & ~same_color
    if np.any(active):
        scene_mean = np.average(ramp[active], weights=counts[active])
        gamma = np.clip(.73 + 2.8 * (scene_mean - .415), .73, 1.0)
        ramp = ramp ** gamma
    if np.count_nonzero(border == backdrop) > border.size * .60:
        ramp[same_color] = 0
    return ramp


def quantize_bg(values):
    """Small ordered dots on broad midtones; no stipple on strong contours."""
    v = np.asarray(values, dtype=float) / 255
    y, x = np.indices(v.shape)
    padded = np.pad(v, 1, mode="edge")
    neighbors = [padded[1:-1, :-2], padded[1:-1, 2:], padded[:-2, 1:-1], padded[2:, 1:-1]]
    edge = np.maximum.reduce([np.abs(v - other) for other in neighbors])
    strength = np.clip(1 - 3 * edge, 0, 1) * .42
    return np.clip(np.floor((1 - v) * 3 + .5 + strength * BAYER[y % 4, x % 4]), 0, 3).astype(np.uint8)


def fit_tiles(screen, budget):
    """Frequency-weighted farthest medoids in decoded pixel space; preserve exact tiles when possible."""
    h, w = screen.shape
    cells = screen.reshape(h // 8, 8, w // 8, 8).transpose(0, 2, 1, 3).reshape(-1, 64)
    unique, inverse, counts = np.unique(cells, axis=0, return_inverse=True, return_counts=True)
    if len(unique) <= budget:
        chosen = np.arange(len(unique))
        mapping = inverse.ravel()
    else:
        # Preserve internal edges as well as mean tone when selecting representatives.
        pixels = unique.astype(np.float32)
        blocks = pixels.reshape(-1, 8, 8)
        features = np.concatenate([pixels, .45 * np.diff(blocks, axis=1).reshape(-1, 56),
                                   .45 * np.diff(blocks, axis=2).reshape(-1, 56)], axis=1)
        nearest = np.full(len(unique), np.inf, dtype=np.float32)
        indices = np.zeros(len(unique), dtype=int)
        chosen = []
        candidate = int(counts.argmax())
        weights = counts ** .65
        for i in range(budget):
            chosen.append(candidate)
            delta = features - features[candidate]
            dist = np.einsum("ij,ij->i", delta, delta)
            better = dist < nearest
            indices[better] = i
            nearest = np.minimum(dist, nearest)
            candidate = int(np.argmax(nearest * weights))
        mapping = indices[inverse.ravel()]
    patterns = unique[chosen].reshape(-1, 8, 8)
    display = patterns[mapping].reshape(h // 8, w // 8, 8, 8).transpose(0, 2, 1, 3).reshape(h, w)
    return patterns, mapping.astype(np.uint8), display, len(unique)


def actor_pose(frame, size=(24, 32), scale=.85):
    """Keep the silhouette and small features when fitting the three opaque OBJ inks."""
    dest = np.zeros((size[1], size[0]), np.uint8)
    if frame is None or frame.getbbox() is None:
        return dest
    frame = frame.crop(frame.getbbox())
    ratio = min(scale, size[0] / frame.width, size[1] / frame.height)
    new_size = (max(1, round(frame.width * ratio)), max(1, round(frame.height * ratio)))
    filtered = np.asarray(frame.resize(new_size, Image.Resampling.LANCZOS), dtype=float)
    opaque = filtered[:, :, 3] >= 116
    if not np.any(opaque):
        return dest
    # Filter silhouette coverage. Within it, retain the source texel's
    # contribution so a one-pixel eye or rib survives the smaller scale.
    nearest = np.asarray(frame.resize(new_size, Image.Resampling.NEAREST), dtype=float)
    mix = np.where(nearest[:, :, 3:] >= 128, .70 * nearest + .30 * filtered, filtered)
    rgb = mix[:, :, :3]
    light = lightness(rgb)
    # Red cloth and brown leather lie close in L*. Deep red receives the
    # darker ink, leaving Django's face and scarf visibly separate.
    red = np.clip((rgb[:, :, 0] - np.maximum(rgb[:, :, 1], rgb[:, :, 2])) / 175, 0, 1)
    light -= .18 * red * (rgb[:, :, 1] < 100)
    lo, hi = np.percentile(light[opaque], [6, 94])
    tone = (light - lo) / max(hi - lo, .15)
    inks = np.where(tone < .30, 3, np.where(tone < .77, 2, 1))
    # Eight-neighbor silhouette would overwhelm narrow limbs. Ink only the
    # dark boundary samples and preserve bright headgear and weapon highlights.
    mask = np.pad(opaque, 1)
    interior = mask[1:-1, :-2] & mask[1:-1, 2:] & mask[:-2, 1:-1] & mask[2:, 1:-1]
    inks[opaque & ~interior & (tone < .62)] = 3
    x = (size[0] - new_size[0]) // 2
    dest[-new_size[1]:, x:x + new_size[0]] = np.where(opaque, inks, 0)
    return dest


def pose_tiles(pose):
    """8x16 OBJ mode: consecutive even/odd tile pairs for each hardware object."""
    h, w = pose.shape
    return b"".join(encode_tile(pose[y + dy:y + dy + 8, x:x + 8])
                    for y in range(0, h, 16) for x in range(0, w, 8) for dy in (0, 8))


def pose_image(pose, enemy=False):
    reg = OBP1 if enemy else OBP0
    shades = np.array([(reg >> (i * 2)) & 3 for i in range(4)])
    im = screen_image(shades[pose]).convert("RGBA")
    im.putalpha(Image.fromarray((pose != 0).astype("uint8") * 255))
    return im
