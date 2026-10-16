#!/usr/bin/env python3
"""Large sprites (resource type 98F5, one 1.7 MB entry): decode and render.

  bigsprites.py index [--palfile OBJPAL.bin]   first frame of every sprite
  bigsprites.py sheet HASH [--palfile F]       every frame of one sprite
  bigsprites.py list                           hash, size, frame count

Layout (offsets relative to the resource; inferred, confidence medium):
  +0x04 u32 count; +0x0C u32 pixel data start; +0x10 u32 frame table;
  +0x14 u32 part table.
  +0x18 count records of 12 B: u32 name hash (as requested by
        ScriptProc_Start(0xCB05, name) in actor code), u8 width, u8 height,
        u8 centre x, u8 centre y, u32 byte offset into the frame table.
        A sprite's frames run to the next record's offset.
  frame, 12 B: u8 parts, u8 tiles, u16 palette (the default 9B1B pool entry,
        the same in every frame; AnimData_Load applies it), u32 pixel offset (from the
        pixel data start; 4bpp tiles, consumed by the parts in order),
        u32 byte offset into the part table.
  part, 4 B: u8 size << 2 | shape (GBA OBJ), u8 flags, s8 x, s8 y.
Output is ROM-derived and stays under build/assets/.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import archive  # noqa: E402
import sprites  # noqa: E402
from romlib import ROOT, load_rom  # noqa: E402


class BigSprites:
    def __init__(self, rom):
        for label, _, _, ents in archive.archives(rom):
            if label == "t98F5":
                _, a, s = ents[0]
                self.d = rom[a - 0x08000000:a - 0x08000000 + s]
        d = self.d
        self.count, = struct.unpack_from("<I", d, 4)
        self.pixels, self.frames_at, self.parts_at = struct.unpack_from("<3I", d, 0xC)
        self.records = [struct.unpack_from("<IBBBBI", d, 0x18 + i * 12) for i in range(self.count)]

    def frames(self, i):
        start = self.records[i][5]
        end = self.records[i + 1][5] if i + 1 < self.count else self.parts_at - self.frames_at
        for o in range(start, end, 12):
            yield struct.unpack_from("<BBHII", self.d, self.frames_at + o)

    def render(self, frame, pal):
        from PIL import Image
        nparts, ntiles, palette, pix, po = frame
        parts = []
        tile = 0
        for k in range(nparts):
            attr, fl, x, y = struct.unpack_from("<BBbb", self.d, self.parts_at + po + k * 4)
            w, h = sprites.SIZES.get(attr & 3, sprites.SIZES[0])[(attr >> 2) & 3]
            parts.append((x, y, w, h, tile, fl))
            tile += (w // 8) * (h // 8)
        if not parts:
            return None
        x0 = min(p[0] for p in parts)
        y0 = min(p[1] for p in parts)
        x1 = max(p[0] + p[2] for p in parts)
        y1 = max(p[1] + p[3] for p in parts)
        img = Image.new("RGBA", (x1 - x0, y1 - y0), (0, 0, 0, 0))
        px = img.load()
        colours = pal[0] if isinstance(pal[0], list) else pal
        for x, y, w, h, t0, fl in reversed(parts):
            tw = w // 8
            for ty in range(h // 8):
                for tx in range(tw):
                    o = self.pixels + pix + (t0 + ty * tw + tx) * 32
                    for r in range(8):
                        for c in range(8):
                            v = (self.d[o + r * 4 + c // 2] >> (4 * (c & 1))) & 15
                            if v:
                                px[x - x0 + tx * 8 + c, y - y0 + ty * 8 + r] = colours[v] + (255,)
        return img


def palette_arg():
    if "--palfile" in sys.argv:
        return sprites.obj_palettes(sys.argv[sys.argv.index("--palfile") + 1])
    return sprites.gray()


def main():
    from PIL import Image, ImageDraw
    rom = load_rom()
    bs = BigSprites(rom)
    cmd = sys.argv[1] if len(sys.argv) > 1 else "list"
    pal = palette_arg()
    out_dir = os.path.join(ROOT, "build", "assets", "bigsprites")
    os.makedirs(out_dir, exist_ok=True)
    if cmd == "list":
        for i, r in enumerate(bs.records):
            print(f"{i:3} {r[0]:04X} {r[1]}x{r[2]} frames={len(list(bs.frames(i)))}")
    elif cmd == "index":
        cols, cell = 16, 72
        rows = (bs.count + cols - 1) // cols
        sheet = Image.new("RGBA", (cols * cell, rows * (cell + 10)), (40, 40, 60, 255))
        dr = ImageDraw.Draw(sheet)
        for i, r in enumerate(bs.records):
            fr = next(bs.frames(i), None)
            im = bs.render(fr, pal) if fr else None
            x, y = (i % cols) * cell, (i // cols) * (cell + 10)
            if im:
                if max(im.size) > cell - 4:
                    k = (cell - 4) / max(im.size)
                    im = im.resize((max(1, int(im.width * k)), max(1, int(im.height * k))))
                sheet.alpha_composite(im, (x + (cell - im.width) // 2, y + 2 + (cell - 4 - im.height) // 2))
            dr.text((x + 2, y + cell - 2), f"{r[0]:04X}", fill=(255, 255, 0, 255))
        out = os.path.join(out_dir, "index.png")
        sheet.resize((sheet.width * 2, sheet.height * 2), Image.NEAREST).save(out)
        print(out)
    elif cmd == "sheet":
        h = int(sys.argv[2], 16)
        i = next(k for k, r in enumerate(bs.records) if r[0] == h)
        imgs = [im for im in (bs.render(f, pal) for f in bs.frames(i)) if im]
        cw = max(im.width for im in imgs) + 4
        ch = max(im.height for im in imgs) + 4
        cols = min(10, len(imgs))
        sheet = Image.new("RGBA", (cols * cw, ((len(imgs) + cols - 1) // cols) * ch), (40, 40, 60, 255))
        for n, im in enumerate(imgs):
            sheet.alpha_composite(im, ((n % cols) * cw + 2, (n // cols) * ch + 2))
        out = os.path.join(out_dir, f"{h:04X}.png")
        sheet.resize((sheet.width * 3, sheet.height * 3), Image.NEAREST).save(out)
        print(out)


if __name__ == "__main__":
    main()
