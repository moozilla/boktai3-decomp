import math
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from datafmt import math_lookup, to_bytes


class MathLookups(unittest.TestCase):
    def test_sources_reproduce_original_angle_conventions(self):
        for name, start, count, code, generate in [
            ("sine", 0x086149C4, 320, "h", lambda i: int(4096 * math.sin(math.tau * i / 256))),
            ("atan", 0x08614C44, 257, "B", lambda i: int(128 * math.atan(i / 256) / math.pi)),
        ]:
            with self.subTest(name=name):
                source = (ROOT / "data" / "math" / (name + ".csv")).read_text()
                values = [generate(i) for i in range(count)]
                expected = struct.pack(f"<{count}{code}", *values)
                items = math_lookup.encode(source, start, start + len(expected))
                self.assertTrue(all(not pointer for _, _, pointer in items))
                self.assertEqual(to_bytes(items), expected)

    def test_lun_rates_roundtrip_as_unsigned_words(self):
        start = 0x08605E48
        bank = struct.pack("<8I", 0, 1, 127, 256, 65535, 65536, 0x80000000, 0xFFFFFFFF)
        end = start + len(bank)
        rom = bytes(start - math_lookup.ROM_BASE) + bank
        source = math_lookup.decode(rom, start, end, {})
        self.assertEqual(source.splitlines()[0], "phase,rate")
        self.assertEqual(to_bytes(math_lookup.encode(source, start, end)), bank)

    def test_unsigned_effect_bank_roundtrips_and_rejects_bad_indices(self):
        start = 0x086141BC
        bank = struct.pack("<1024H", *range(65535, 65535 - 1024, -1))
        end = start + len(bank)
        rom = bytes(start - math_lookup.ROM_BASE) + bank
        source = math_lookup.decode(rom, start, end, {})
        self.assertEqual(to_bytes(math_lookup.encode(source, start, end)), bank)
        for invalid in [source.replace("0,65535", "1,65535"), source.replace("0,65535", "0,-1"),
                        source.rsplit("\n", 2)[0]]:
            with self.assertRaises(ValueError):
                math_lookup.encode(invalid, start, end)
