import sys
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[1] / "tools"
sys.path[:0] = [str(TOOLS / "scope"), str(TOOLS)]
from unit_check import unit_matches


class UnitCheck(unittest.TestCase):
    def test_requires_rom_bytes_full_extent_and_internal_boundaries(self):
        start = 0x08000000
        rom = b"ababcd"
        expected = {"A": (start, 2), "B": (start + 2, 4)}
        offsets = {"A": 0, "B": 2}
        self.assertTrue(unit_matches(rom, start, offsets, expected, rom))
        self.assertFalse(unit_matches(b"ababad", start, offsets, expected, rom))
        self.assertFalse(unit_matches(rom[:-2], start, offsets, expected, rom))
        self.assertFalse(unit_matches(rom, start, {"A": 0}, expected, rom))
        self.assertFalse(unit_matches(rom, start, {"A": 2, "B": 0}, expected, rom))
