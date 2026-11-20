"""Round-trip an embedded 4bpp MP background, including shared and flipped tiles."""
import struct
from PIL import Image


class TileMap:
    def __init__(self, data, patterns=None):
        if data[:2] != b"MP":
            raise ValueError("Expected an uncompressed MP map")
        count, ntiles, nmeta, self.tile_base = struct.unpack_from("<4H", data, 4)
        table, pixels, defs = struct.unpack_from("<3I", data, 12)
        if not ntiles or (not pixels and patterns is None):
            raise ValueError("This map requires an external pattern table")
        self.header = data[:24]
        sheet = data[pixels:pixels+ntiles*32] if pixels else patterns
        if len(sheet) < ntiles*32:
            raise ValueError("The active pattern set does not cover the map")
        self.tiles = [sheet[i*32:(i+1)*32] for i in range(ntiles)]
        self.metas = [struct.unpack_from("<4H", data, defs + i*8) for i in range(nmeta)]
        self.layers = []
        for i in range(count):
            w, h, tag, offset = struct.unpack_from("<HHII", data, table + i*12)
            if w*h > 65536:
                raise ValueError("Invalid native map dimensions")
            stored = struct.unpack_from(f"<{w*h}H", data, offset)
            order = self.grid_order(w, h)
            grid = [stored[slot] for slot in order]
            self.layers.append((w, h, tag, grid))

    @staticmethod
    def grid_order(w, h):
        # Each 16x16 chunk is contiguous in ROM, even on a wide world map.
        if w % 16 or h % 16:
            return range(w*h)
        return [((y//16)*(w//16)+x//16)*256 + (y%16)*16 + x%16
                for y in range(h) for x in range(w)]

    def render(self, index):
        """Return native 8-bit palette indices, including each tile's palette bank."""
        w, h, _, grid = self.layers[index]
        image = Image.new("P", (16*w, 16*h))
        out = image.load()
        for i, meta in enumerate(grid):
            for q, tile in enumerate(self.metas[meta]):
                slot = (tile & 1023) - self.tile_base
                if not 0 <= slot < len(self.tiles):
                    raise ValueError("A layer references tiles outside its decoded sheet")
                data = self.tiles[slot]
                x0, y0 = (i%w)*16 + (q%2)*8, (i//w)*16 + (q//2)*8
                for y in range(8):
                    for x in range(8):
                        sx = 7-x if tile & 1024 else x
                        sy = 7-y if tile & 2048 else y
                        nibble = (data[sy*4 + sx//2] >> (4*(sx%2))) & 15
                        out[x0+x, y0+y] = (tile >> 12)*16 + nibble
        return image

    def replace(self, index, original, rendered, *, private_layer=False, preserve_other_layers=False,
                limit=1024, fixed_tiles=(), recycle_unused=False):
        """Copy on write. A private HUD upload may reuse an exclusively edited slot."""
        w, h, _, grid = self.layers[index]
        if original.size != rendered.size or original.size != (w*16, h*16):
            raise ValueError("Map dimensions must be preserved")
        changes = {}
        demand = {}
        for i, mid in enumerate(grid):
            for q, entry in enumerate(self.metas[mid]):
                slot = (entry & 1023) - self.tile_base
                x, y = (i%w)*16 + (q%2)*8, (i//w)*16 + (q//2)*8
                box = (x, y, x+8, y+8)
                tile = rendered.crop(box)
                pixels = tile.tobytes()
                if pixels == original.crop(box).tobytes():
                    desired = self.tiles[slot]
                else:
                    if len({p >> 4 for p in pixels}) != 1:
                        raise ValueError("A BG tile must use one palette bank")
                    if entry & 1024:
                        tile = tile.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
                    if entry & 2048:
                        tile = tile.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
                    pixels = tile.tobytes()
                    desired = bytes((a & 15) | ((b & 15) << 4)
                                    for a, b in zip(pixels[::2], pixels[1::2]))
                    changes[i,q] = desired
                if private_layer:
                    demand.setdefault(slot, set()).add(desired)
        if preserve_other_layers:
            for other, (_, _, _, cells) in enumerate(self.layers):
                if other == index:
                    continue
                for mid in set(cells):
                    for entry in self.metas[mid]:
                        slot = (entry & 1023) - self.tile_base
                        if slot in demand:
                            demand[slot].add(self.tiles[slot])
        for slot, desired in demand.items():
            if slot not in fixed_tiles and self.tiles[slot] not in desired:
                self.tiles[slot] = min(desired)
        # A single-layer upload can borrow tiles belonging only to other map
        # variants. Keep fixed native digit and animation slots at their IDs.
        free = set()
        if recycle_unused and private_layer and not preserve_other_layers:
            visible = {(entry & 1023) - self.tile_base for m in grid for entry in self.metas[m]}
            free = set(range(len(self.tiles))) - visible - set(fixed_tiles)
        tile_ids = {tile: i+self.tile_base for i,tile in enumerate(self.tiles) if i not in free}
        available = sorted(free)
        meta_ids = {m: i for i,m in enumerate(self.metas)}
        for i, mid in enumerate(grid):
            entries = list(self.metas[mid])
            for q, entry in enumerate(entries):
                desired = changes.get((i,q))
                if desired is None:
                    continue
                if desired not in tile_ids:
                    if available:
                        slot = available.pop()
                        new_id = slot + self.tile_base
                        self.tiles[slot] = desired
                    else:
                        new_id = len(self.tiles) + self.tile_base
                        if new_id >= limit:
                            raise ValueError(f"Replacement exceeds the loader's {limit}-tile budget")
                        self.tiles.append(desired)
                    tile_ids[desired] = new_id
                entries[q] = (entry & ~1023) | tile_ids[desired]
            encoded = tuple(entries)
            if encoded not in meta_ids:
                meta_ids[encoded] = len(self.metas)
                self.metas.append(encoded)
            grid[i] = meta_ids[encoded]

    def pattern_sheet(self, bank):
        """Native 16-column patterns also feed tiles assembled directly in C."""
        im = Image.new("P", (128, ((len(self.tiles)+15)//16)*8), bank*16)
        for i, tile in enumerate(self.tiles):
            indices = bytes(bank*16+n for b in tile for n in (b & 15, b >> 4))
            im.paste(Image.frombytes("P", (8, 8), indices), ((i%16)*8, (i//16)*8))
        return im

    def update_patterns(self, original, edited):
        for i in range((original.height//8)*16):
            x, y = (i%16)*8, (i//16)*8
            box = x, y, x+8, y+8
            pixels = edited.crop(box).tobytes()
            if original.crop(box).tobytes() != pixels:
                if i >= len(self.tiles):
                    raise ValueError("Lettering extends beyond the native pattern table")
                self.tiles[i] = bytes((a & 15) | ((b & 15) << 4) for a,b in zip(pixels[::2],pixels[1::2]))

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
            stored = [0]*len(grid)
            for cell, slot in enumerate(self.grid_order(w, h)):
                stored[slot] = grid[cell]
            grids.extend(struct.pack(f"<{len(grid)}H", *stored))
        return bytes(header+table+tiles+metas+grids)


def palette(data):
    values = struct.unpack_from("<256H", data, 20)
    return [((v >> shift) & 31)*255//31 for v in values for shift in (0, 5, 10)]


def text_mask(size, text, font_size, style="sampled", line_gap=3):
    """Use the same line breaks and size for model feedback and encoded tiles."""
    from PIL import ImageDraw
    from graphics import font, serif

    width, height = size
    for pixels in range(font_size, min(6, font_size - 1), -1):
        face = serif(pixels) if style == "serif" else font(pixels)
        lines = []
        for paragraph in text.splitlines():
            row = ""
            for word in paragraph.split():
                trial = (row + " " + word).strip()
                if row and face.getlength(trial) > width:
                    lines.append(row)
                    row = word
                else:
                    row = trial
            lines.append(row)
        line_height = pixels + line_gap
        if (len(lines) * line_height - line_gap <= height
                and all(face.getlength(line) <= width for line in lines)):
            break
    else:
        raise ValueError(f"The translated lettering does not fit the {width} by {height} pixel region")
    mask = Image.new("L", size)
    pen = ImageDraw.Draw(mask)
    for i, line in enumerate(lines):
        if line:
            pen.text((0, i * line_height), line, 255, font=face, anchor="lt")
    if style in ("left", "right", "center", "serif") and mask.getbbox():
        ink = mask.crop(mask.getbbox())
        x = {"left": 0, "right": width-ink.width}.get(style, (width-ink.width)//2)
        y = (height-ink.height)//2
        mask = Image.new("L", size)
        if style == "serif":
            mask.paste(ink.point(lambda v: v//3), (x+1,y+2))
        mask.paste(ink, (x,y), ink)
    return mask


def typeset(image, box, colors, text, size, style="sampled"):
    """Preserve each original 4bpp tile's palette bank under the new glyphs."""
    from collections import Counter, defaultdict

    crop = image.crop(box)
    source = crop.tobytes()
    mask = text_mask(crop.size, text, size, style)
    banks = defaultdict(Counter)
    for index in source:
        banks[index >> 4][index] += 1

    def rgb(i):
        return colors[i*3:i*3+3]

    def distance(a, b):
        return sum((u-v)**2 for u, v in zip(a, b))

    ramps = {}
    for bank, inks in banks.items():
        background = inks.most_common(1)[0][0]
        others = inks if len(inks) > 1 else range(bank*16, bank*16+16)
        foreground = max(others, key=lambda i: distance(rgb(i), rgb(background)))
        ramp = []
        for alpha in range(256):
            blend = [(a*(255-alpha) + b*alpha)//255
                     for a, b in zip(rgb(background), rgb(foreground))]
            ramp.append(min(others, key=lambda i: distance(blend, rgb(i))))
        ramps[bank] = ramp
    recolored = bytes(ramps[index >> 4][alpha] for index, alpha in zip(source, mask.tobytes()))
    image.paste(Image.frombytes("P", crop.size, recolored), tuple(box[:2]))
