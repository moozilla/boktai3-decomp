"""Marker numeric tables retain order, signed offsets, fixed extents and bytes."""
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
from datafmt import actor_marker_table as codec, to_bytes


def fixture(start):
    table = codec.TABLES[start]
    if table['format'] == 'hhhH':
        values = [(-32768 + i, 32767 - i, -i, 65535) for i in range(18)]
    else:
        values = [(i % 2 if 'animation' in table['columns'] else i,) for i in range(18)]
    raw = b''.join(struct.pack('<' + table['format'], *row) for row in values)
    rom = bytes(start - codec.ROM_BASE) + raw
    return codec.decode(rom, start, table['end'], {}), raw


class ActorMarkerTableTests(unittest.TestCase):
    def test_all_three_tables_roundtrip_as_numeric_items(self):
        for start, table in codec.TABLES.items():
            with self.subTest(start=start):
                text, raw = fixture(start)
                items = codec.encode(text, start, table['end'])
                self.assertEqual(to_bytes(items), raw)
                self.assertTrue(all(pointer is False for _, _, pointer in items))
                rows = codec.parse_rows(text, start, table['end'])
                self.assertEqual(len(rows), 18)
                self.assertEqual(list(rows[-1].values())[:2], [1, 8])

    def test_edits_change_encoded_values(self):
        start = 0x0824E818
        text, raw = fixture(start)
        edited = text.replace('0,0,-32768,32767,0,65535', '0,0,-123,32767,0,65535')
        got = to_bytes(codec.encode(edited, start, codec.TABLES[start]['end']))
        self.assertEqual(struct.unpack_from('<h', got)[0], -123)
        self.assertEqual(got[2:], raw[2:])

    def test_rows_cannot_resize_or_reorder(self):
        start = 0x0824E6D4
        text, _ = fixture(start)
        lines = text.splitlines()
        variants = [lines[:-1], lines + [lines[-1]],
                    lines[:1] + lines[2:3] + lines[1:2] + lines[3:]]
        for variant in variants:
            with self.subTest(variant=variant), self.assertRaises(ValueError):
                codec.encode('\n'.join(variant), start, codec.TABLES[start]['end'])

    def test_invalid_ranges_columns_and_values_are_rejected(self):
        for start, table in codec.TABLES.items():
            text, _ = fixture(start)
            for a, b in ((start + 1, table['end']), (start, table['end'] + 1)):
                with self.subTest(start=a, end=b), self.assertRaises(ValueError):
                    codec.encode(text, a, b)
            with self.assertRaises(ValueError):
                codec.encode(text.replace('fragment', 'index'), start, table['end'])
            with self.assertRaises(ValueError):
                codec.decode(b'', start, table['end'], {})
        for start, before, after in ((0x0824E6D4, '0,0,0', '0,0,256'),
                                      (0x0824E6D4, '0,0,0', '0,0,-1'),
                                      (0x0824E6E6, '0,0,0', '0,0,2'),
                                      (0x0824E818, '-32768', '-32769'),
                                      (0x0824E818, '65535', '65536')):
            text, _ = fixture(start)
            with self.subTest(start=start, value=after), self.assertRaises(ValueError):
                codec.encode(text.replace(before, after, 1), start, codec.TABLES[start]['end'])

    def test_pointer_like_frame_bytes_stay_bytes(self):
        start = 0x0824E6D4
        raw = bytes([1, 2, 3, 4, 5, 6, 7, 8] + [0] * 10)
        text = codec.decode(bytes(start - codec.ROM_BASE) + raw,
                            start, codec.TABLES[start]['end'], {})
        items = codec.encode(text, start, codec.TABLES[start]['end'])
        self.assertEqual(to_bytes(items), raw)
        self.assertEqual(items[4:8], [(1, value, False) for value in (5, 6, 7, 8)])


if __name__ == '__main__':
    unittest.main()
