"""Resolve a specific sprite frame as the engine selects it for a widget."""
from collections import Counter
from hashlib import sha256
from io import BytesIO
import json
from pathlib import Path
import struct
import tempfile

from PIL import Image
import archive
import sprites
from graphics import BASE, REGIONS, SIZES, font, letter_mask, lettering, palette_pool, quantize, rectangles
from runtime import HERE, OUT, ROOT
from translator import PROVIDER
from vision import ArtIndex

MARKED = json.loads((HERE / "regions.json").read_text())


def write_cache(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=path.parent, delete=False) as f:
        f.write(data)
        tmp = Path(f.name)
    tmp.replace(path)


def png(image):
    out = BytesIO()
    image.save(out, format="PNG")
    return out.getvalue()


def foreground(region):
    """Estimate backing and ink from an original label, retaining alpha."""
    border = list(region.crop((0, 0, region.width, 1)).getdata())
    border += list(region.crop((0, region.height-1, region.width, region.height)).getdata())
    counts = Counter(region.getdata())
    bg = Counter(border).most_common(1)[0][0]
    opaque = [color for color in counts if color[3] and color != bg]
    if not opaque:
        return bg, (255, 255, 255, 255)
    if not bg[3]:
        return bg, max(opaque, key=counts.get)
    def contrast(color):
        distance = sum((a-b)**2 for a,b in zip(color, bg))
        return distance * min(1, counts[color] / max(2, region.width/4))
    return bg, max(opaque, key=contrast)


def paint_region(image, box, text):
    region = image.crop(box)
    bg, ink = foreground(region)
    mask = letter_mask(text, region.width, min(region.height, max(6, region.height-1)))
    region.paste(bg, (0, 0, region.width, region.height))
    region.paste(ink, ((region.width-mask.width)//2, (region.height-mask.height)//2), mask)
    image.paste(region, tuple(box[:2]))


class FrameEncoder:
    """Allocate legal OBJ parts on a 16-tile-wide sheet for a single glyph."""
    def __init__(self, package, index, palettes):
        self.pkg = package
        self.index = index
        self.frame = list(package.frames())[index]
        self.parts = list(package.parts(self.frame))
        self.left = min(p["x"] for p in self.parts)
        self.top = min(p["y"] for p in self.parts)
        self.banks = sorted({p["palette"] for p in self.parts})
        self.palettes = palettes
        self.descriptors = bytearray()
        self.tiles = bytearray()

    def add(self, x, y, w, h, bank, indices):
        if not any(indices):
            return
        first = len(self.tiles)//32
        ntiles = (h//8-1)*16+w//8
        if first+ntiles > 4096:
            raise ValueError("Frame exceeds the 12-bit OBJ tile limit")
        self.tiles.extend(bytes(ntiles*32))
        for py in range(h):
            for px in range(0, w, 2):
                tile = first + (py//8)*16 + px//8
                offset = tile*32 + (py%8)*4 + (px%8)//2
                self.tiles[offset] = indices[py*w+px] | indices[py*w+px+1]<<4
        shape, size = SIZES[w,h]
        self.descriptors.extend(struct.pack("<HhhH", shape<<8 | size<<10, x, y, first | bank<<12))

    def original_outside(self, boxes):
        # Retain each original palette index, flip and z order outside the
        # lettering. Overlapping parts may use different banks at the same pixel.
        for part in self.parts:
            w, h = part["w"], part["h"]
            x0, y0 = part["x"]-self.left, part["y"]-self.top
            indices = [0]*(w*h)
            for ty in range(h//8):
                for tx in range(w//8):
                    rows = self.pkg.pixel_rows(part["tile"]+ty*16+tx)
                    for y, row in enumerate(rows):
                        for x, color in enumerate(row):
                            dx, dy = tx*8+x, ty*8+y
                            if part["hflip"]: dx = w-1-dx
                            if part["vflip"]: dy = h-1-dy
                            if not any(a <= x0+dx < c and b <= y0+dy < d for a,b,c,d in boxes):
                                indices[dy*w+dx] = color
            self.add(part["x"], part["y"], w, h, part["palette"], indices)

    def encode(self, image, boxes=None):
        if boxes:
            self.original_outside(boxes)
            patch = Image.new("RGBA", image.size)
            for box in boxes:
                patch.paste(image.crop(box), tuple(box[:2]))
            image = patch
        for x, y, w, h in rectangles(image):
            nearest = quantize(image.crop((x,y,x+w,y+h)), self.palettes, self.banks)
            if nearest:
                self.add(self.left+x, self.top+y, w, h, *nearest)
        count = len(self.descriptors)//8
        if count > 128:
            raise ValueError("Frame exceeds the hardware OBJ count")
        pos = self.pkg.offs[0] + 20*self.index
        entry = bytearray(self.pkg.data[pos:pos+20])
        struct.pack_into("<H", entry, 2, count)
        struct.pack_into("<I", entry, 16, 0)
        parts_at = 32+len(entry)
        tiles_at = parts_at+len(self.descriptors)
        header = struct.pack("<6H5I", self.pkg.h0, 1, 0, count, 0, len(self.tiles)//32,
                             32, parts_at, parts_at, tiles_at, tiles_at)
        return header+entry+self.descriptors+self.tiles


class ArtTranslator:
    def __init__(self):
        self.tables = None
        self.pool = None
        self.loaded = {}
        self.packages = {}
        self.index = None

    def initialize(self, game, memory):
        self.index = ArtIndex(memory)
        self.pool = palette_pool(game.rom)
        self.tables = {}
        for _, category, _, entries in archive.archives(game.rom):
            if category == 0xCB05:
                for key, addr, size in entries:
                    offset = struct.unpack_from("<I", game.rom, addr-BASE+12)[0]
                    self.tables[addr+offset] = key, addr-BASE, size

    def select(self, resolver, game, index, header, table, lr, scene, proc, frame):
        if self.tables is None:
            self.initialize(game, resolver.memory)
        record = self.tables.get(table)
        if not record:
            return 0
        key, start, size = record
        if (key,index) in self.loaded:
            return self.loaded[key,index]
        if key not in self.packages:
            pkg = sprites.Package(game.rom[start:start+size])
            self.packages[key] = pkg, list(pkg.frames())
        pkg, frames = self.packages[key]
        if index >= len(frames) or not frames[index]["parts"]:
            return 0
        code = f"{key:04X}"
        asset = f"graphic/{code}/{index}"
        self.loaded[key,index] = 0
        palettes = self.pool[pkg.h0:pkg.h0+16]
        ctx = resolver.catalog.context(game, scene, proc, lr, "graphic", "", None)
        ctx.update(archive="CB05", package=code, frame_index=index, glyph_id=frames[index]["id"],
                   palette_banks=sorted({p["palette"] for p in pkg.parts(frames[index])}))
        recipe = REGIONS.get(code, {}).get(str(index))
        gallery = OUT / "graphics"
        gallery.mkdir(parents=True, exist_ok=True)
        path = gallery / f"{code}-{index}-ja.png"
        try:
            image = pkg.render(frames[index], palettes)
            ctx.update(width_px=image.width, height_px=image.height)
            if not path.exists():
                write_cache(path, png(image))
            self.index.encounter(asset, ctx, str(path.relative_to(ROOT)))
            resolver.record(asset, event="selected", frame=frame, **ctx)
            if not resolver.enabled:
                if recipe:
                    resolver.record(asset, event="japanese", source=recipe[0], frame=frame, **ctx)
                return 0
            if recipe:
                source, style, suggestion = recipe
                regions = [{"id": "", "style": style, "source": source, "english": suggestion,
                            "origin": PROVIDER, "box": [0, 0, image.width, image.height]}]
            else:
                state = self.index.status(asset)["state"]
                if state == "unexamined" and asset in MARKED:
                    self.index.mark(asset, MARKED[asset], PROVIDER)
                elif state == "unexamined" and resolver.memory.model:
                    guesses = resolver.memory.model.locate(ctx, resolver.memory.glossary(), png(image))
                    self.index.mark(asset, guesses, resolver.memory.model.model)
                regions = self.index.regions(asset)
            if not regions:
                return 0
            choices = []
            for region in regions:
                rid = region["id"]
                entry = asset + ("/"+rid if rid else "")
                box = region["box"]
                layout = dict(ctx, region=box, width_px=box[2]-box[0], height_px=box[3]-box[1],
                              typography=region.get("style", "sampled"))
                english, origin, event = resolver.memory.choose(entry, region["source"], layout,
                    png(image.crop(box)), region.get("english"), region["origin"])
                resolver.record(entry, source=region["source"], english=english, provenance=origin,
                                event=event, frame=frame, **layout)
                if english:
                    choices.append((region, english))
            if not choices:
                return 0
            signature = [6, ctx["rom_sha256"], asset, choices, font(16).getname()]
            digest = sha256(json.dumps(signature, sort_keys=True).encode()).hexdigest()
            path = OUT / "compiled" / (digest+".cb05")
            if path.exists():
                blob = path.read_bytes()
                event = "tile-cache"
            else:
                if recipe:
                    region, english = choices[0]
                    result = lettering(image, english, region["style"])
                    boxes = None
                else:
                    result = image.copy()
                    boxes = []
                    for region, english in choices:
                        paint_region(result, region["box"], english)
                        boxes.append(region["box"])
                blob = FrameEncoder(pkg, index, palettes).encode(result, boxes)
                write_cache(path, blob)
                event = "typeset"
            localized = sprites.Package(blob)
            preview = gallery / f"{code}-{index}-en.png"
            if event == "typeset" or not preview.exists():
                preview_image = localized.render(next(localized.frames()), palettes)
                write_cache(preview, png(preview_image))
            base = game.install(blob)
            # The original function expects a relocated header. Its only lookup
            # uses index zero in this private one-frame resource.
            relocated = bytearray(blob[:32])
            struct.pack_into("<5I", relocated, 12, *(base+off for off in localized.offs))
            address = game.install(bytes(relocated))
            self.loaded[key,index] = address
            resolver.record(asset, event=event, digest=digest, frame=frame, **ctx)
            return address
        except (ValueError, OSError, IndexError, struct.error) as exc:
            self.index.error(asset, str(exc))
            resolver.record(asset, event="needs-review", reason=str(exc), frame=frame, **ctx)
            return 0
