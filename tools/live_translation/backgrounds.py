"""Expose decoded map layers at the point their native loader uploads the tiles."""
import csv
from hashlib import sha256
import json
import struct

from runtime import HERE, OUT, ROOT
from graphics import BASE
from tilemap import TileMap, palette, typeset
from vision import ArtIndex, png, write_cache
import archive


class MapTranslator:
    def __init__(self):
        self.specs = json.loads((HERE/"backgrounds.json").read_text())
        self.addresses = {}
        self.palettes = {}
        self.maps = {}
        self.pattern_libraries = {}
        self.pattern_loads = {}
        self.loaded = {}
        self.index = None
        with (ROOT / "data/stage_palettes.csv").open() as f:
            self.stage_colors = {row["map"]: row["palette"] for row in csv.DictReader(f)}

    def initialize(self, game, memory):
        self.index = ArtIndex(memory)
        for _, category, _, entries in archive.archives(game.rom):
            if category == 0xC091:
                self.addresses.update((addr, (f"{key:04X}", size)) for key, addr, size in entries)
                self.maps = {f"{key:04X}": (addr, size) for key, addr, size in entries}
            if category == 0xCEEF:
                self.pattern_libraries = {addr: key for key, addr, _ in entries}
            if category == 0x92B3:
                self.palettes = {f"{key:04X}": addr for key, addr, _ in entries}

    def pattern(self, resolver, game, packed, ptr, ids_at):
        if self.index is None:
            self.initialize(game, resolver.memory)
        if ptr not in self.pattern_libraries:
            return
        layer, start, count = packed & 3, (packed >> 2) & 1023, packed >> 16
        ids = struct.unpack(f"<{count}H", game.read(ids_at, count*2))
        rom = game.rom
        base = ptr-BASE
        entries, indices, pool = struct.unpack_from("<3I", rom, base+12)
        total = struct.unpack_from("<H", rom, base)[0]
        pages = {sid: (n, offset) for sid,n,offset in
                 struct.iter_unpack("<HHI", rom[base+entries:base+entries+total*8])}
        sheet = bytearray()
        for sid in ids:
            if sid not in pages:
                return
            n, offset = pages[sid]
            for tile, in struct.iter_unpack("<H", rom[base+indices+offset:base+indices+offset+n*2]):
                at = base+pool+tile*32
                sheet.extend(rom[at:at+32])
        destination = 0x8000 if layer == 0 else start*32
        self.pattern_loads[destination] = (bytes(sheet), [f"{sid:04X}" for sid in ids])

    def select(self, resolver, game, index, ptr, mode, lr, scene, proc, frame, *, hud=False):
        if self.index is None:
            self.initialize(game, resolver.memory)
        if ptr == 0x02020404:
            # Bg_LoadMap's decompression cache retains its source hash in IWRAM.
            code = f'{int.from_bytes(game.read(0x03001548,2), "little"):04X}'
            resource = self.maps.get(code)
            if not resource:
                return 0
            from tilesets import unpack_map
            addr, size = resource
            raw, _ = unpack_map(game.rom[addr-BASE:addr-BASE+size])
            if game.read(ptr, 24) != raw[:24]:
                return 0
        else:
            record = self.addresses.get(ptr)
            if not record:
                return 0
            code, size = record
            raw = game.rom[ptr-BASE:ptr-BASE+size]
        if raw[:2] != b"MP":
            return 0
        patterns = None
        active_sets = []
        external = struct.unpack_from("<I",raw,16)[0] == 0
        asset = f"background/{code}/{index}"
        if external:
            ntiles, _, first = struct.unpack_from("<HHH", raw, 6)
            start = 0x8000 if mode in (0,6) else first*32
            patterns, active_sets = self.pattern_loads.get(start, (None, []))
            if patterns is None or len(patterns) < ntiles*32:
                return 0
            # Verify the source archive interpretation against the engine's DMA,
            # before offering it to the translator or replacing a map.
            if patterns[:ntiles*32] != game.read(0x06000000+start, ntiles*32):
                return 0
        if active_sets:
            asset += "@" + "+".join(active_sets)
        signature = (code, index, mode, tuple(active_sets))
        if signature in self.loaded:
            return self.loaded[signature]
        self.loaded[signature] = 0
        ctx = resolver.catalog.context(game, scene, proc, lr, "graphic", "", None)
        ctx.update(archive="C091", layer=index, resource=code)
        if active_sets:
            ctx.update(pattern_archive="CEEF", tilesets=active_sets, verified_native_upload=True)
        spec = self.specs.get(code, {}).get(str(index), {})
        try:
            doc = TileMap(raw, patterns)
            if not 0 <= index < len(doc.layers):
                return 0
            w, h, _, _ = doc.layers[index]
            if w*h > 8192:
                resolver.record(asset, event="large-map", width_px=w*16, height_px=h*16, frame=frame, **ctx)
                return 0
            image = doc.render(index)
            family = self.specs.get(code, {})
            palette_name = (spec.get("palette") or self.stage_colors.get(code) or
                            next((s["palette"] for s in family.values() if "palette" in s), None))
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
            variant = "-"+("+".join(active_sets)) if active_sets else ""
            stem = f"MP-{code}-{index}{variant}-{palette_name.replace(' ', '-')}"
            path = gallery / (stem+"-ja.png")
            selected = self.index.translate(resolver, asset, image, ctx, path, spec.get("regions", []), frame)
            choices = [(r["box"], word, r.get("font") or min(15,r["box"][3]-r["box"][1])) for r,word in selected]
            if not choices:
                return 0
            cache_key = [5, active_sets, ctx["rom_sha256"], asset, palette_name, colors, choices, hud, mode if not hud else None]
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
                if external:
                    limit = doc.tile_base + len(doc.tiles)
                doc.replace(index, image, edited, private_layer=hud or external,
                            preserve_other_layers=external, limit=limit)
                compiled = doc.pack()
                write_cache(cache, compiled)
                write_cache(gallery / (stem+"-en.png"), png(edited))
                event = "typeset"
            self.loaded[signature] = game.install(compiled)
            resolver.record(asset, event=event, digest=digest, frame=frame, **ctx)
        except (ValueError, OSError, IndexError, struct.error) as exc:
            self.index.error(asset, str(exc))
            resolver.record(asset, event="needs-review", reason=str(exc), frame=frame, **ctx)
        return self.loaded[signature]
