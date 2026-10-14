#!/usr/bin/env python3
"""Sprite packages (archive type CB05): decode and render frames.

  sprites.py index [--palfile F]             one labelled frame per package
  sprites.py info HASH                       header and section counts
  sprites.py sheet HASH [OUT.png] [--palfile OBJPAL.bin]
                                             contact sheet of every frame; colours
                                             from a dumped OBJ palette RAM (05000200)

Package layout (offsets relative to the package; ResourceHeader_Relocate
082196C4 rebases the five offset words):
  +0x00 u16 h0 (unknown), u16 frames, u16 animations, u16 parts, u16 keys, u16 tiles
  +0x0C frame table      20 B: u16 id, u16 part count, u32 ?, s16 x, s16 y,
                               u16 w, u16 h, u32 byte offset into the part table
  +0x10 animation table   8 B: u16 id, u16 key count, u32 byte offset into the key table
  +0x14 part table        8 B: u16 attr (size << 10 | shape << 8 | vflip 8 | hflip 4),
                               s16 x, s16 y, u16 OAM attr2 (tile 0-9, priority 10-11, palette 12-15)
  +0x18 key table         4 B: u16 frame index, u16 duration
  +0x1C tiles             4bpp, 32 B each, a 2D sheet 16 tiles wide (an OBJ's
                          rows are 16 tiles apart)
Field roles are inferred from layout and counts (confidence: medium); the
palette is not in the package (default: grayscale).
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import archive  # noqa: E402
from romlib import ROOT, load_rom  # noqa: E402

SHEET_STRIDE = 16  # tiles are stored as a 2D sheet, 16 tiles per row
SIZES = {0: [(8, 8), (16, 16), (32, 32), (64, 64)],
         1: [(16, 8), (32, 8), (32, 16), (64, 32)],
         2: [(8, 16), (8, 32), (16, 32), (32, 64)]}


class Package:
    def __init__(self, data):
        self.data = data
        self.h0, self.nframes, self.nanims, self.nparts, self.nkeys, self.ntiles = struct.unpack_from("<6H", data, 0)
        self.offs = struct.unpack_from("<5I", data, 0xC)

    def frames(self):
        for i in range(self.nframes):
            fid, n, unk, x, y, w, h, po = struct.unpack_from("<HHIhhHHI", self.data, self.offs[0] + i * 20)
            yield dict(id=fid, parts=n, unk=unk, x=x, y=y, w=w, h=h, part_offset=po)

    def parts(self, frame):
        base = self.offs[2] + frame["part_offset"]
        for i in range(frame["parts"]):
            attr, x, y, tile = struct.unpack_from("<HhhH", self.data, base + i * 8)
            w, h = SIZES[(attr >> 8) & 3][(attr >> 10) & 3]
            yield dict(x=x, y=y, w=w, h=h, tile=tile & 0x3FF, palette=tile >> 12, priority=(tile >> 10) & 3,
                       hflip=bool(attr & 4), vflip=bool(attr & 8))

    def animations(self):
        for i in range(self.nanims):
            aid, n, ko = struct.unpack_from("<HHI", self.data, self.offs[1] + i * 8)
            keys = [struct.unpack_from("<HH", self.data, self.offs[3] + ko + k * 4) for k in range(n)]
            yield dict(id=aid, keys=keys)

    def pixel_rows(self, tile):
        o = self.offs[4] + tile * 32
        t = self.data[o:o + 32]
        return [[(t[r * 4 + c // 2] >> (4 * (c & 1))) & 15 for c in range(8)] for r in range(8)]

    def render(self, frame, pal):
        from PIL import Image
        parts = list(self.parts(frame))
        if not parts:
            return None
        x0 = min(p["x"] for p in parts)
        y0 = min(p["y"] for p in parts)
        x1 = max(p["x"] + p["w"] for p in parts)
        y1 = max(p["y"] + p["h"] for p in parts)
        img = Image.new("RGBA", (x1 - x0, y1 - y0), (0, 0, 0, 0))
        px = img.load()
        for p in reversed(parts):  # first part on top
            tw = p["w"] // 8
            for ty in range(p["h"] // 8):
                for tx in range(tw):
                    tile = p["tile"] + ty * SHEET_STRIDE + tx
                    if tile >= self.ntiles:
                        continue
                    rows = self.pixel_rows(tile)
                    for r in range(8):
                        for c in range(8):
                            v = rows[r][c]
                            if not v:
                                continue
                            dx, dy = tx * 8 + c, ty * 8 + r
                            if p["hflip"]:
                                dx = p["w"] - 1 - dx
                            if p["vflip"]:
                                dy = p["h"] - 1 - dy
                            colours = pal[p["palette"]] if isinstance(pal[0], list) else pal
                            px[p["x"] - x0 + dx, p["y"] - y0 + dy] = colours[v] + (255,)
        return img


def gray():
    """False colours: a hue per index, so shading ramps stay readable."""
    import colorsys
    out = [(0, 0, 0)]
    for i in range(1, 16):
        r, g, b = colorsys.hsv_to_rgb((i * 0.37) % 1.0, 0.55, 0.45 + 0.55 * i / 15)
        out.append((int(r * 255), int(g * 255), int(b * 255)))
    return out


def obj_palettes(path):
    """16 palettes of 16 colours from a 512-byte OBJ palette RAM dump."""
    raw = open(path, "rb").read()
    def rgb(v):
        return ((v & 31) * 255 // 31, (v >> 5 & 31) * 255 // 31, (v >> 10 & 31) * 255 // 31)
    return [[rgb(struct.unpack_from("<H", raw, (p * 16 + i) * 2)[0]) for i in range(16)] for p in range(16)]


def package(rom, h):
    for label, _, _, ents in archive.archives(rom):
        if label == "tCB05":
            for eh, a, s in ents:
                if eh == h:
                    return Package(rom[a - 0x08000000:a - 0x08000000 + s])
    raise SystemExit(f"no sprite package {h:04X}")


def sheet(pkg, pal, cols=12, scale=2):
    from PIL import Image
    imgs = [im for im in (pkg.render(f, pal) for f in pkg.frames()) if im]
    if not imgs:
        return None
    cap = 160  # a few frames spread parts over the screen; shrink those to fit
    imgs = [im if max(im.size) <= cap else
            im.resize((max(1, im.width * cap // max(im.size)), max(1, im.height * cap // max(im.size))))
            for im in imgs]
    cw = max(i.width for i in imgs) + 4
    ch = max(i.height for i in imgs) + 4
    cols = min(cols, len(imgs))
    rows = (len(imgs) + cols - 1) // cols
    out = Image.new("RGBA", (cols * cw, rows * ch), (40, 40, 60, 255))
    for n, im in enumerate(imgs):
        out.alpha_composite(im, ((n % cols) * cw + 2, (n // cols) * ch + 2))
    return out.resize((out.width * scale, out.height * scale), Image.NEAREST)


def main():
    rom = load_rom()
    if sys.argv[1] == "index":
        pal = obj_palettes(sys.argv[sys.argv.index("--palfile") + 1]) if "--palfile" in sys.argv else gray()
        out = os.path.join(ROOT, "build", "assets", "sprites", "index.png")
        os.makedirs(os.path.dirname(out), exist_ok=True)
        index_sheet(rom, pal, out)
        print(out)
        return
    cmd, h = sys.argv[1], int(sys.argv[2], 16)
    pkg = package(rom, h)
    if cmd == "info":
        print(f"h0={pkg.h0} frames={pkg.nframes} animations={pkg.nanims} parts={pkg.nparts} "
              f"keys={pkg.nkeys} tiles={pkg.ntiles}")
    elif cmd == "sheet":
        out = sys.argv[3] if len(sys.argv) > 3 and not sys.argv[3].startswith("--") else \
            os.path.join(ROOT, "build", "assets", "sprites", f"{h:04X}.png")
        os.makedirs(os.path.dirname(out), exist_ok=True)
        pal = gray()
        if "--palfile" in sys.argv:
            pal = obj_palettes(sys.argv[sys.argv.index("--palfile") + 1])
        im = sheet(pkg, pal)
        if im:
            im.save(out)
            print(out)



def index_sheet(rom, pal, out, scale=2, cols=8, cell=96):
    """One labelled frame (the largest of the first animation) per package."""
    from PIL import Image, ImageDraw
    pkgs = []
    for label, _, _, ents in archive.archives(rom):
        if label == "tCB05":
            pkgs = [(h, Package(rom[a - 0x08000000:a - 0x08000000 + s])) for h, a, s in ents]
    rows = (len(pkgs) + cols - 1) // cols
    sheet_img = Image.new("RGBA", (cols * cell, rows * (cell + 10)), (40, 40, 60, 255))
    d = ImageDraw.Draw(sheet_img)
    for n, (h, pkg) in enumerate(pkgs):
        frames = list(pkg.frames())
        best = None
        for f in frames[:24]:
            im = pkg.render(f, pal)
            if im and max(im.size) <= 200 and (best is None or im.width * im.height > best.width * best.height):
                best = im
        x, y = (n % cols) * cell, (n // cols) * (cell + 10)
        if best:
            if max(best.size) > cell - 4:
                k = (cell - 4) / max(best.size)
                best = best.resize((max(1, int(best.width * k)), max(1, int(best.height * k))))
            sheet_img.alpha_composite(best, (x + (cell - best.width) // 2, y + 2 + (cell - 4 - best.height) // 2))
        d.text((x + 2, y + cell - 2), f"{h:04X} f{pkg.nframes}", fill=(255, 255, 0, 255))
    sheet_img.resize((sheet_img.width * scale, sheet_img.height * scale), Image.NEAREST).save(out)


if __name__ == "__main__":
    main()
