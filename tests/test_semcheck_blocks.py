"""Coverage must not count literal data or padding as Thumb blocks."""
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import semcheck


class BlockCoverage(unittest.TestCase):
    def test_resume_after_literal_pool_without_decoding_its_branch_bytes(self):
        start = semcheck.ROM_BASE
        # b +8, alignment, branch-shaped literal, beq +14, bx lr, padding, bx lr
        code = bytes.fromhex("02e0 0000 00d0 ffff 01d0 7047 0000 7047")
        with patch.object(semcheck, "rom_data_ranges", return_value=([(2, 8)], [])):
            blocks = semcheck.rom_blocks(code, start, start + len(code), "fixture")
        self.assertEqual(blocks, {start + n for n in (0, 8, 10, 14)})

    def test_unconditional_branch_has_no_fallthrough(self):
        start = semcheck.ROM_BASE
        code = bytes.fromhex("00e0 0000 7047")
        self.assertEqual(semcheck.rom_blocks(code, start, start + len(code)), {start, start + 4})
