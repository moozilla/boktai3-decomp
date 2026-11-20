"""Expose decoded map layers at the point their native loader uploads the tiles."""
import csv
from hashlib import sha256
import json
import struct

from runtime import HERE, OUT, ROOT
from graphics import BASE
from frames import png, write_cache
from tilemap import TileMap, palette, typeset
from translator import PROVIDER
from vision import ArtIndex
import archive


class MapTranslator:
    def __init__(self):
        self.specs = json.loads((HERE/"backgrounds.json").read_text())
        self.addresses = {}
        self.palettes = {}
        self.loaded = {}
        self.index = None
        with (ROOT / "data/stage_palettes.csv").open() as f:
            self.stage_colors = {row["map"]: row["palette"] for row in csv.DictReader(f)}

    def initialize(self, game, memory):
        self.index = ArtIndex(memory)
        for _, category, _, entries in archive.archives(game.rom):
            if category == 0xC091:
                self.addresses.update((addr, (f"{key:04X}", size)) for key, addr, size in entries)
            if category == 0x92B3:
                self.palettes = {f"{key:04X}": addr for key, addr, _ in entries}

    def select(self, resolver, game, index, ptr, mode, lr, scene, proc, frame, *, hud=False):
        if self.index is None:
            self.initialize(game, resolver.memory)
        record = self.addresses.get(ptr)
        if not record:
            return 0
        code, size = record
        signature = (code, index, mode if hud else None)
        if signature in self.loaded:
            return self.loaded[signature]
        self.loaded[signature] = 0
        raw = game.rom[ptr-BASE: ptr-BASE+size]
        asset = f"background/{code}/{index}"
        if raw[:2] != b"MP" or not struct.unpack_from("<I", raw, 16)[0]:
            return 0
        ctx = resolver.catalog.context(game, scene, proc, lr, "graphic", "", None)
        ctx.update(archive="C091", layer=index, resource=code)
        spec = self.specs.get(code, {}).get(str(index), {})
        try:
            doc = TileMap(raw)
            if not 0 <= index < len(doc.layers):
                return 0
            w, h, _, _ = doc.layers[index]
            if w*h > 8192:
                resolver.record(asset, event="large-map", width_px=w*16, height_px=h*16, frame=frame, **ctx)
                return 0
            image = doc.render(index)
            palette_name = spec.get("palette") or self.stage_colors.get(code)
            if hud:
                # Hud_SetupBackgroundLayer copies the last three banks, beginning
                # at +0x1B4 in a 92B3 palette, just after calling Bg_SetupLayers.
                palette_name = next((key for key, addr in self.palettes.items() if addr+0x1B4 == mode), palette_name)
            if palette_name in self.palettes:
                addr = self.palettes[palette_name]
                colors = palette(game.rom[addr-BASE:addr-BASE+532])
            else:
                # Live palette before upload. Mark its timing in the evidence.
                colors = palette(bytes(20) + game.read(0x03004FC0, 512))
                palette_name = "current BG palette buffer"
            image.putpalette(colors)
            ctx.update(width_px=image.width, height_px=image.height, palette=palette_name)
            gallery = OUT / "graphics"
            path = gallery / f"MP-{code}-{index}-ja.png"
            if not path.exists():
                write_cache(path, png(image))
            self.index.encounter(asset, ctx, str(path.relative_to(ROOT)))
            resolver.record(asset, event="selected", frame=frame, **ctx)
            if not resolver.enabled:
                return 0
            if self.index.status(asset)["state"] == "unexamined":
                if spec.get("regions"):
                    self.index.mark(asset, spec["regions"], PROVIDER)
                elif resolver.memory.model:
                    found = resolver.memory.model.locate(ctx, resolver.memory.glossary(), png(image))
                    self.index.mark(asset, found, resolver.memory.model.model)
            regions = self.index.regions(asset)
            choices = []
            for region in regions:
                box, source = region["box"], region["source"]
                key = asset + "/" + region["id"]
                crop = image.crop(box)
                layout = dict(ctx, region=box, width_px=crop.width, height_px=crop.height)
                word, origin, event = resolver.memory.choose(key, source, layout, png(crop),
                                                            region["english"], region["origin"])
                resolver.record(key, event=event, source=source, english=word, provenance=origin, frame=frame, **layout)
                if word:
                    size_hint = next((r.get("font") for r in spec.get("regions", []) if r["id"] == region["id"]), None)
                    choices.append((box, word, size_hint or min(15, crop.height)))
            if not choices:
                return 0
            cache_key = [4, ctx["rom_sha256"], asset, palette_name, colors, choices, hud, mode if not hud else None]
            digest = sha256(json.dumps(cache_key).encode()).hexdigest()
            cache = OUT / "compiled" / (digest + ".mp")
            if cache.exists():
                compiled = cache.read_bytes()
                event = "tile-cache"
            else:
                edited = image.copy()
                for box, word, height in choices:
                    typeset(edited, box, colors, word, height)
                # The HUD's native font cache starts just after its first 256
                # tiles. The private resource is used for this single selection.
                limit = 256 if hud or mode in (0, 6) else 1024
                doc.replace(index, image, edited, private_layer=hud, limit=limit)
                compiled = doc.pack()
                write_cache(cache, compiled)
                write_cache(gallery / f"MP-{code}-{index}-en.png", png(edited))
                event = "typeset"
            self.loaded[signature] = game.install(compiled)
            resolver.record(asset, event=event, digest=digest, frame=frame, **ctx)
        except (ValueError, OSError, IndexError, struct.error) as exc:
            self.index.error(asset, str(exc))
            resolver.record(asset, event="needs-review", reason=str(exc), frame=frame, **ctx)
        return self.loaded[signature]
