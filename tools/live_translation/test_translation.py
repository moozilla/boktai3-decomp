"""Regression tests for encounter resolution, source preservation and model I/O."""
from io import BytesIO
import json
from pathlib import Path
import struct
import tempfile
import unittest
from unittest.mock import patch

from PIL import Image

from runtime import baserom_path
from translator import Memory, Ollama, fit
from vision import ArtIndex, png
from tilemap import TileMap, typeset


class ResolverTests(unittest.TestCase):
    def test_backend_uses_context_revises_and_remembers_terms(self):
        source = "<LABEL=RATA>???</LABEL>\nジグは太陽を見た。"
        context = dict(kind="dialogue", columns=16, lines=3, scene=6554,
                       speaker_label="RATA", script={"procedure": 21})
        replies = iter([{"english": "Missing controls"},
                        {"english": "<LABEL=RATA>???</LABEL>\nZig saw the Sun.",
                         "terms": {"ジグ": "Zig", "absent": "wrong"}}])
        requests = []

        def transport(req, timeout):
            payload = json.loads(req.data)
            requests.append(json.loads(payload["prompt"]))
            self.assertEqual(req.full_url, "http://model.invalid/api/generate")
            return BytesIO(json.dumps({"response": json.dumps(next(replies))}).encode())

        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp)/"memory.sqlite"
            provider = Ollama("http://model.invalid", "test-provider")
            with patch("urllib.request.urlopen", side_effect=transport):
                memory = Memory(path, provider)
                choice = memory.choose("unit/line", source, context)
                self.assertEqual(choice[1:], ("test-provider", "first"))
                self.assertEqual(requests[0]["context"], context)
                self.assertEqual(requests[0]["glossary"]["ジャンゴ"], "Django")
                self.assertIn("control", requests[1]["revision"])
                self.assertEqual(memory.glossary()["ジグ"], "Zig")
                self.assertNotIn("absent", memory.glossary())
                memory.db.close()
                memory = Memory(path, provider)
                self.assertEqual(memory.choose("unit/line", source, context)[2], "cache")
                self.assertEqual(len(requests), 2)
                memory.db.close()

    def test_narrower_second_use_keeps_an_independent_cached_wording(self):
        wide = dict(kind="menu", columns=20, lines=1)
        small = dict(kind="menu", columns=6, lines=1)
        source, key = "魔法機械", "unit/device"
        model = Ollama("http://model.invalid", "layout-test")
        mem = Memory(":memory:", model)
        mem.choose(key, source, wide, suggested="Magic machine", author="editor")
        with patch.object(model, "translate", return_value=("Device", {})) as call:
            self.assertEqual(mem.choose(key, source, small), ("Device", "layout-test", "fitted"))
            context = call.call_args.args[1]
            self.assertEqual(context["previous_wording"], "Magic machine")
            self.assertEqual(context["columns"], 6)
        mem.model = None
        self.assertEqual(mem.choose(key, source, small)[0], "Device")
        self.assertEqual(mem.choose(key, source, wide)[0], "Magic machine")
        mem.correct(key, "Engine")
        self.assertEqual(mem.choose(key, source, small)[0], "Engine")
        mem.db.close()

    def test_speaker_name_and_graphic_geometry_survive_editing(self):
        from editor import editable_body
        original = "<LABEL=OTENKO>???</LABEL>\nAwake, <NAME>?"
        label, body = editable_body(original)
        self.assertEqual(body, "Awake, <NAME>?")
        self.assertEqual(label + body, original)
        layout = dict(kind="graphic", archive="CB05", width_px=112, height_px=8, typography="buttons")
        with self.assertRaises(ValueError):
            fit("Ⓐけってい　Ⓑもどる", "Confirm", layout)
        fit("Ⓐけってい　Ⓑもどる", "Choose | Back", layout)

    def test_failing_provider_leaves_an_editable_encounter(self):
        ctx = dict(kind="menu", columns=6, lines=1)
        provider = Ollama("http://model.invalid", "test-provider")
        memory = Memory(":memory:", provider)
        with patch("urllib.request.urlopen", side_effect=OSError("offline")) as call:
            for _ in range(2):
                with self.assertRaises(OSError):
                    memory.choose("unit/one", "知らない", ctx)
            self.assertEqual(call.call_count, 1)
        memory.correct("unit/one", "Unsure")
        self.assertEqual(memory.choose("unit/one", "知らない", ctx), ("Unsure", "human correction", "cache"))
        memory.db.close()

    def test_new_vision_region_reaches_memory_and_can_be_remarked(self):
        image = Image.new("RGBA", (32, 16))
        crop = png(image)
        ctx = dict(kind="graphic", width_px=32, height_px=16, scene=2511)
        provider = Ollama("http://model.invalid", "test-vision")

        def transport(req, timeout):
            import base64
            payload = json.loads(req.data)
            self.assertEqual(base64.b64decode(payload["images"][0]), crop)
            self.assertEqual(json.loads(payload["prompt"])["context"]["scene"], 2511)
            response = {"regions":[{"box":[3,2,28,12], "source":"入口", "english":"Entry"}]}
            return BytesIO(json.dumps({"response":json.dumps(response)}).encode())

        memory = Memory(":memory:", provider)
        index = ArtIndex(memory)
        asset = "object/0001/0"
        index.encounter(asset, ctx, "build/example.png")
        with patch("urllib.request.urlopen", side_effect=transport):
            regions = provider.locate(ctx, memory.glossary(), crop)
        index.mark(asset, regions, provider.model)
        region, = index.regions(asset)
        key = asset+"/r0"
        self.assertEqual(memory.choose(key, "入口", ctx, crop, region["english"], region["origin"])[0], "Entry")
        index.mark(asset, [dict(region, english="Entrance")], "human correction")
        self.assertEqual(memory.choose(key, "入口", ctx), ("Entrance", "human correction", "cache"))
        with self.assertRaises(ValueError):
            index.mark(asset, [dict(region, box=[-1,2,11,10])], "human correction")
        memory.db.close()

    def test_reflow_counts_substitutions_and_preserves_control_stream(self):
        ctx = dict(kind="dialogue", columns=12, lines=2)
        self.assertEqual(fit("<NAME>へ", "Hello <NAME>!", ctx), "Hello\n<NAME>!")
        with self.assertRaises(ValueError):
            fit("<PROC=2>はい", "Yes!", ctx)
        with self.assertRaises(ValueError):
            fit("元気?", "Hi <bad", ctx)
        centered = fit("　　太陽　　", "Sun", ctx)
        self.assertEqual(centered, "    Sun")


class GraphicTests(unittest.TestCase):
    def test_map_lettering_can_cross_banks_without_changing_a_tile_bank(self):
        original = Image.new("P", (32, 12), 17)
        original.paste(35, (16, 0, 32, 12))
        original.putpixel((0, 2), 18)
        original.putpixel((18, 3), 36)
        colors = [0]*768
        for i in (18, 36):
            colors[3*i:3*i+3] = [255, 255, 255]
        edited = original.copy()
        typeset(edited, (0, 0, 32, 12), colors, "Gate", 10)
        self.assertNotEqual(edited.tobytes(), original.tobytes())
        self.assertEqual([i >> 4 for i in original.tobytes()], [i >> 4 for i in edited.tobytes()])

    def test_shared_map_edit_retains_other_layers_and_flips(self):
        header = struct.pack("<4s4H3I", b"MP\0\0", 2, 1, 1, 0, 24, 48, 80)
        tables = struct.pack("<HHIIHHII", 1,1,11,88, 1,1,12,90)
        tile = bytes((i*13)%256 for i in range(32))
        meta = struct.pack("<4H", 0x1000, 0x1400, 0x1800, 0x1c00)
        original = header+tables+tile+meta+bytes(4)
        doc = TileMap(original)
        first = doc.render(0)
        second = doc.render(1)
        edit = first.copy()
        edit.putpixel((9,13), 21)
        doc.replace(0, first, edit, private_layer=True, preserve_other_layers=True)
        recovered = TileMap(doc.pack())
        self.assertEqual(recovered.render(0).tobytes(), edit.tobytes())
        self.assertEqual(recovered.render(1).tobytes(), second.tobytes())

    def test_obj_region_preserves_every_pixel_outside_lettering(self):
        from frames import FrameEncoder, paint_region
        from graphics import palette_pool
        import sprites
        rom = Path(baserom_path()).read_bytes()
        package = sprites.package(rom, 0x530D)
        index = 114
        frame = list(package.frames())[index]
        palettes = palette_pool(rom)[package.h0:package.h0+16]
        original = package.render(frame, palettes)
        edited = original.copy()
        box = [5,2,64,12]
        paint_region(edited, box, "Map")
        converted = sprites.Package(FrameEncoder(package, index, palettes).encode(edited, [box]))
        final = converted.render(next(converted.frames()), palettes)
        self.assertEqual(original.size, final.size)
        outside = final.copy()
        outside.paste(original.crop(box), tuple(box[:2]))
        self.assertEqual(outside.tobytes(), original.tobytes())
        self.assertNotEqual(final.crop(box).tobytes(), original.crop(box).tobytes())


if __name__ == "__main__":
    unittest.main()
