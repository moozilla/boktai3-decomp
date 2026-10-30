"""Ordered script dispatch tables keep their indices, extents and relocations."""
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
from datafmt import callback_array, to_bytes


class CallbackArrayTests(unittest.TestCase):
    def setUp(self):
        self.start = callback_array.ROM_BASE + 4
        self.end = self.start + 12
        self.values = (0x080837D5, 0, 0x08136EB1)
        self.raw = struct.pack('<3I', *self.values)
        self.rom = b'head' + self.raw + b'tail'
        self.text = callback_array.decode(
            self.rom, self.start, self.end, {0x080837D4: 'Lift'})

    def test_roundtrip_preserves_index_and_thumb_bit(self):
        items = callback_array.encode(self.text, self.start, self.end)
        self.assertEqual(to_bytes(items), self.raw)
        self.assertEqual([kind for _, _, kind in items], [True, False, True])
        # Names are review aids; code renames must never change ROM bytes.
        renamed = self.text.replace('Lift', 'RenamedLift')
        self.assertEqual(callback_array.encode(renamed, self.start, self.end), items)

    def test_reordering_or_resizing_is_rejected(self):
        lines = self.text.splitlines()
        cases = [lines[:1] + lines[2:3] + lines[1:2] + lines[3:],
                 lines[:-1], lines + [lines[-1]]]
        for lines in cases:
            with self.subTest(source=lines), self.assertRaises(ValueError):
                callback_array.encode('\n'.join(lines), self.start, self.end)

    def test_invalid_callback_addresses_are_rejected(self):
        for address in ('080837D4', '00000001', '03000001', 'FFFFFFFF', '-1'):
            with self.subTest(address=address), self.assertRaises(ValueError):
                callback_array.encode(self.text.replace('080837D5', address),
                                      self.start, self.end)

    def test_invalid_extents_are_rejected(self):
        for start, end in ((self.start + 1, self.end), (self.start, self.end - 1),
                           (self.end, self.start)):
            with self.subTest(start=start, end=end), self.assertRaises(ValueError):
                callback_array.decode(self.rom, start, end, {})
        with self.assertRaises(ValueError):
            callback_array.decode(self.rom, self.start, self.end + 8, {})

    def test_missing_required_columns_are_rejected(self):
        with self.assertRaises(ValueError):
            callback_array.encode('index,name\n0,Lift\n', self.start, self.end)


if __name__ == '__main__':
    unittest.main()
