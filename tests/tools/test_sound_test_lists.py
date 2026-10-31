import struct
import sys
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from types import SimpleNamespace
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / "tools"), str(ROOT / "tools" / "studio")]
from datafmt import sound_test as codec, to_bytes
import sound


class SoundTestLists(unittest.TestCase):
    def test_menu_order_and_duplicate_song_positions(self):
        entries = []
        with TemporaryDirectory() as d:
            for start, (name, size) in codec.LISTS.items():
                end = start + 2 * (size + 1)
                raw = struct.pack(f"<{size + 1}h", 7, 7, *range(100, 100 + size - 2), -1)
                source = codec.decode(bytes(start - codec.ROM_BASE) + raw, start, end, {})
                self.assertEqual(to_bytes(codec.encode(source, start, end)), raw)
                path = Path(d) / (name + ".csv")
                path.write_text(source)
                entries.append(SimpleNamespace(start=start, end=end, format="sound_test", file=path))
            with patch.object(sound, "load_manifest", return_value=entries):
                positions = sound.sound_test()
        self.assertEqual(positions[7], [
            {"list": name, "no": number}
            for name in ("music", "effects", "voices") for number in (1, 2)
        ])

    def test_terminator_and_row_validation(self):
        start, size = 0x086136F0, 94
        end = start + (size + 1) * 2
        rom = bytes(start - codec.ROM_BASE) + bytes(2 * size)
        with self.assertRaises(ValueError):
            codec.decode(rom + b"\x00\x00", start, end, {})
        text = codec.decode(rom + b"\xff\xff", start, end, {})
        for bad in (text.replace("1,0\n", "1,-1\n", 1), text.replace("1,0\n", "", 1)):
            with self.assertRaises(ValueError):
                codec.encode(bad, start, end)
