"""Reviewed interior labels must not truncate C replacement or byte credit."""
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
import progress
import split


class NonentryTests(unittest.TestCase):
    def test_preserve_interior_label_and_full_extent(self):
        code = ['\tthumb_func_start sub_08060708',
                'sub_08060708: @ 0x08060708', '\tpush {r4, r5, r6, lr}',
                '\tmov r6, r8', '\tthumb_func_start sub_0806070C',
                'sub_0806070C: @ 0x0806070C', '\tpush {r6}',
                '\tthumb_func_start sub_080607B0',
                'sub_080607B0: @ 0x080607B0']
        with tempfile.TemporaryDirectory() as tmp:
            source = Path(tmp) / 'code.s'
            source.write_text('\n'.join(code))
            filtered = split.remove_nonentry_starts(code, {0x0806070C})
            self.assertIn('sub_0806070C: @ 0x0806070C', filtered)
            self.assertIn('\tpush {r6}', filtered)
            self.assertNotIn('\tthumb_func_start sub_0806070C', filtered)
            self.assertEqual(len(filtered), len(code) - 1)
            inventory = Path(tmp) / 'inventory.csv'
            symbols = Path(tmp) / 'functions.csv'
            progress.write_inventory(inventory, [0x08060708, 0x080607B0])
            symbols.write_text('addr,name\n')
            _, sizes = progress.load_functions(inventory, symbols)
            self.assertEqual(sizes['sub_08060708'], 168)

    def test_boundaries_derive_from_the_inventory(self):
        with patch.object(split, 'inventory_starts', return_value=[0x08060708, 0x08060790, 0x080607B0]), \
             patch.object(split, 'disassembler_starts', return_value={0x08060708, 0x0806070C, 0x080607B0}):
            self.assertEqual(split.function_nonentries(), {0x0806070C})
            self.assertEqual(split.extra_function_starts(), {0x08060790})
