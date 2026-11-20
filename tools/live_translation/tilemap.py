"""Round-trip an embedded 4bpp MP background, including shared and flipped tiles."""
import struct
from PIL import Image


class TileMap:
    def __init__(self, data):
        if data[:2] != b"MP":
            raise ValueError("Expected an uncompressed MP map")
        count, ntiles, nmeta, self.tile_base = struct.unpack_from("<4H", data, 4)
        table, pixels, defs = struct.unpack_from("<3I", data, 12)
        if not pixels or not ntiles:
            raise ValueError("This map uses an external pattern table")
        self.header = data[:24]
        self.tiles = [data[pixels + i*32:pixels + (i+1)*32] for i in range(ntiles)]
        self.metas = [struct.unpack_from("<4H", data, defs + i*8) for i in range(nmeta)]
        self.layers = []
        for i in range(count):
            w, h, tag, offset = struct.unpack_from("<HHII", data, table + i*12)
            grid = list(struct.unpack_from(f"<{w*h}H", data, offset))
            self.layers.append((w, h, tag, grid))

    def render(self, index):
        """Return native 8-bit palette indices, including each tile's palette bank."""
        w, h, _, grid = self.layers[index]
        image = Image.new("P", (16*w, 16*h))
        out = image.load()
        for i, meta in enumerate(grid):
            for q, tile in enumerate(self.metas[meta]):
                data = self.tiles[(tile & 1023) - self.tile_base]
                x0, y0 = (i%w)*16 + (q%2)*8, (i//w)*16 + (q//2)*8
                for y in range(8):
                    for x in range(8):
                        sx = 7-x if tile & 1024 else x
                        sy = 7-y if tile & 2048 else y
                        nibble = (data[sy*4 + sx//2] >> (4*(sx%2))) & 15
                        out[x0+x, y0+y] = (tile >> 12)*16 + nibble
        return image

    def replace(self, index, original, rendered):
        """Copy on write only changed tiles/metatiles; preserve all other users."""
        w, h, _, grid = self.layers[index]
        if original.size != rendered.size or original.size != (w*16, h*16):
            raise ValueError("Map dimensions must be preserved")
        tile_ids = {value: i + self.tile_base for i, value in enumerate(self.tiles)}
        meta_ids = {value: i for i, value in enumerate(self.metas)}
        for i, mid in enumerate(grid):
            entries = list(self.metas[mid])
            for q in range(4):
                x = (i%w)*16 + (q%2)*8
                y = (i//w)*16 + (q//2)*8
                box = (x, y, x+8, y+8)
                pixels = rendered.crop(box).tobytes()
                if pixels == original.crop(box).tobytes():
                    continue
                banks = {p >> 4 for p in pixels}
                if len(banks) != 1:
                    raise ValueError("A BG tile must use one original palette bank")
                packed = bytes((a & 15) | ((b & 15) << 4) for a, b in zip(pixels[::2], pixels[1::2]))
                if packed not in tile_ids:
                    tile_ids[packed] = len(self.tiles) + self.tile_base
                    if tile_ids[packed] >= 1024:
                        raise ValueError("Replacement exceeds the GBA tile index limit")
                    self.tiles.append(packed)
                entries[q] = (banks.pop() << 12) | tile_ids[packed]
            encoded = tuple(entries)
            if encoded not in meta_ids:
                meta_ids[encoded] = len(self.metas)
                self.metas.append(encoded)
            grid[i] = meta_ids[encoded]

    def pack(self):
        table = bytearray()
        tiles = b"".join(self.tiles)
        metas = b"".join(struct.pack("<4H", *m) for m in self.metas)
        grids = bytearray()
        header = bytearray(self.header)
        pos = len(header) + 12*len(self.layers)
        struct.pack_into("<HH", header, 6, len(self.tiles), len(self.metas))
        struct.pack_into("<3I", header, 12, len(header), pos, pos+len(tiles))
        for w, h, tag, grid in self.layers:
            table.extend(struct.pack("<HHII", w, h, tag, pos+len(tiles)+len(metas)+len(grids)))
            grids.extend(struct.pack(f"<{len(grid)}H", *grid))
        return bytes(header+table+tiles+metas+grids)


def palette(data):
    values = struct.unpack_from("<256H", data, 20)
    return [((v >> shift) & 31)*255//31 for v in values for shift in (0, 5, 10)]


def typeset(image, box, colors, text, size, line_gap=3):
    """Fit new lettering inside a plaque using its native bank and ink ramp."""
    from collections import Counter
    from PIL import ImageDraw
    from graphics import font

    x0, y0, x1, y1 = box
    crop = image.crop(box)
    bg, _ = Counter(crop.tobytes()).most_common(1)[0]
    bank = bg // 16
    if any((p >> 4) != bank for p in crop.tobytes()):
        raise ValueError("The lettering region crosses palette banks")
    old_colors = set(crop.tobytes())
    fg = max(old_colors, key=lambda i: sum((colors[3*i+k]-colors[3*bg+k])**2 for k in range(3)))
    for height in range(size, 6, -1):
        face = font(height)
        lines = []
        for paragraph in text.splitlines():
            row = ""
            for word in paragraph.split():
                trial = (row + " " + word).strip()
                if row and face.getlength(trial) > crop.width:
                    lines.append(row)
                    row = word
                else:
                    row = trial
            lines.append(row)
        line_height = height + line_gap
        if len(lines)*line_height-line_gap <= crop.height and all(face.getlength(s) <= crop.width for s in lines):
            break
    else:
        raise ValueError("The translated graphic does not fit")
    mask = Image.new("L", crop.size)
    draw = ImageDraw.Draw(mask)
    for i, line in enumerate(lines):
        draw.text((0, i*line_height), line, 255, font=face, anchor="lt")
    ramp = []
    for alpha in range(256):
        rgb = [(a*(255-alpha)+b*alpha)//255 for a,b in zip(colors[3*bg:3*bg+3],colors[3*fg:3*fg+3])]
        ramp.append(min(old_colors,key=lambda p: sum((a-b)**2 for a,b in zip(rgb,colors[3*p:3*p+3]))))
    image.paste(Image.frombytes("P", crop.size, bytes(ramp[v] for v in mask.tobytes())), (x0,y0))
