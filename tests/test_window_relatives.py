import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from window_relatives import Function, ROM_BASE, Windows


class WindowRelativesTest(unittest.TestCase):
    def test_rank_finds_a_clean_donor_inside_an_unrelated_function(self):
        sequence = [0x2001, 0x280A, 0xDD00, 0x200A, 0x4903, 0x1800, 0x7001]
        variant = [0x2101, 0x290A, 0xDD00, 0x210A, 0x4A03, 0x1800, 0x7001]
        data = sequence + [0x4770, 0xB510] + variant + [0xBD10] + sequence + sequence
        funcs = [Function(ROM_BASE + lo * 2, ROM_BASE + hi * 2, name, name + ".c", clean)
                 for lo, hi, name, clean in ((0, 8, "target", False), (8, 17, "donor", True),
                                             (17, 24, "constrained", False), (24, 31, "pool", True))]
        windows = Windows(struct.pack("<" + "H" * len(data), *data), funcs,
                          [(ROM_BASE, ROM_BASE + 48)])
        hits = windows.rank(ROM_BASE, ROM_BASE + 14)
        self.assertEqual([(h[-1].name, -h[-2]) for h in hits], [("donor", ROM_BASE + 18)])
        with self.assertRaises(ValueError):
            windows.rank(ROM_BASE + 12, ROM_BASE + 20)
        with self.assertRaises(ValueError):
            windows.rank(ROM_BASE + 48, ROM_BASE + 62)

    def test_rejects_half_an_interworking_call(self):
        data = [0x2000, 0xF000, 0xF802, 0x2800, 0xD001, 0x2100, 0x2200, 0xF000, 0xF810]
        end = ROM_BASE + len(data) * 2
        table = Windows(struct.pack("<9H", *data), [Function(ROM_BASE, end, "fn", "fn.c", False)],
                        [(ROM_BASE, end)])
        with self.assertRaisesRegex(ValueError, "first halfword"):
            table.rank(ROM_BASE + 4, end)
        with self.assertRaisesRegex(ValueError, "both halfwords"):
            table.rank(ROM_BASE, end - 2)
