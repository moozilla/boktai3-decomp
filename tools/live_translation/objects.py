"""Translate visible 98F5 actor frames before the native OBJ tile allocator."""
from hashlib import sha256
import json
import struct

from runtime import OUT
from graphics import BASE
from frames import MARKED, paint_region
from vision import ArtIndex, png, write_cache
import archive
import bigsprites
import sprites


class ActorArt:
    def __init__(self, game, memory):
        self.library = bigsprites.BigSprites(game.rom)
        base = next(ents[0][1] for _, typ, _, ents in archive.archives(game.rom) if typ == 0x98F5)
        self.pixel_base = base+self.library.pixels
        self.frames = {}
        self.loaded = {}
        self.index = ArtIndex(memory)
        for i, record in enumerate(self.library.records):
            first = base+self.library.frames_at+record[5]
            for j, frame in enumerate(self.library.frames(i)):
                self.frames[first+j*12] = record[0], j, frame

    def encode(self, frame, source, edited, colors):
        # Each part consumes sequential 4bpp tiles. Retain all native piece
        # dimensions and offsets and change only painted pixel indices.
        count, size, _, offset, parts_at = frame
        lib = self.library
        start = lib.pixels+offset
        tiles = bytearray(lib.d[start:start+size*32])
        parts = []
        for i in range(count):
            tag, _, x, y = struct.unpack_from("<BBbb", lib.d, lib.parts_at+parts_at+4*i)
            w, h = sprites.SIZES[tag & 3][(tag>>2)&3]
            parts.append((x,y,w,h))
        left = min(p[0] for p in parts)
        top = min(p[1] for p in parts)
        old, new = source.load(), edited.load()
        palette = {}
        tile_base = 0
        for x, y, w, h in parts:
            for py in range(h):
                for px in range(w):
                    point = x-left+px, y-top+py
                    color = new[point]
                    if color == old[point]:
                        continue
                    if color not in palette:
                        palette[color] = (min(range(1,16), key=lambda i:
                            sum((a-b)**2 for a,b in zip(color[:3],colors[i]))) if color[3] else 0)
                    tile = tile_base + (py//8)*(w//8) + px//8
                    slot = tile*32 + (py%8)*4 + (px%8)//2
                    shift = (px%2)*4
                    tiles[slot] = (tiles[slot] & ~(15<<shift)) | (palette[color]<<shift)
            tile_base += (w//8)*(h//8)
        return bytes(tiles)

    def select(self, resolver, game, frame_at, pixels, pal_at, lr, scene, proc, tick):
        entry = self.frames.get(frame_at)
        if not entry:
            return 0
        key, index, frame = entry
        if not frame[0] or pixels != self.pixel_base+frame[3]:
            return 0
        identity = (frame_at, pal_at)
        if identity in self.loaded:
            return self.loaded[identity]
        self.loaded[identity] = 0
        code = f"{key:04X}"
        asset = f"object/{code}/{index}"
        ctx = resolver.catalog.context(game, scene, proc, lr, "graphic", "", None)
        ctx.update(archive="98F5", package=code, frame_index=index, default_palette=frame[2],
                   palette_pointer=f"{pal_at:08X}", native_tile_count=frame[1])
        try:
            values = struct.unpack("<16H", game.read(pal_at,32))
            colors = [tuple(((v>>shift)&31)*255//31 for shift in (0,5,10)) for v in values]
            image = self.library.render(frame, colors)
            ctx.update(width_px=image.width, height_px=image.height)
            path = OUT / "graphics" / f"OBJ-{code}-{index}-ja.png"
            choices = self.index.translate(resolver, asset, image, ctx, path, MARKED.get(asset,[]), tick)
            if not choices:
                return 0
            sig = [1, ctx["rom_sha256"], asset, colors, choices]
            digest = sha256(json.dumps(sig, sort_keys=True).encode()).hexdigest()
            cache = OUT / "compiled" / (digest+".obj")
            if cache.exists():
                data = cache.read_bytes()
                event = "tile-cache"
            else:
                edited = image.copy()
                for region, english in choices:
                    paint_region(edited, region["box"], english)
                data = self.encode(frame, image, edited, colors)
                write_cache(cache, data)
                write_cache(OUT / "graphics" / f"OBJ-{code}-{index}-en.png", png(edited))
                event = "typeset"
            if len(data) != frame[1]*32:
                raise ValueError("Actor tile count changed")
            self.loaded[identity] = game.install(data)
            resolver.record(asset, event=event, digest=digest, frame=tick, **ctx)
        except (ValueError, OSError, IndexError, struct.error) as exc:
            self.index.error(asset, str(exc))
            resolver.record(asset, event="needs-review", reason=str(exc), frame=tick, **ctx)
        return self.loaded[identity]
