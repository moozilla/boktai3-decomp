"""struct_views lines up views by offset and flags where they disagree."""
import os
import sys
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "tools"))

import struct_views  # noqa: E402


def leaf(off, name, size=4, signed=False, base="int", ctype="u32", array=False):
    return {"off": off, "name": name, "size": size, "width": size, "signed": signed,
            "base": base, "bits": None, "ctype": ctype, "array": array}


class StructViews(unittest.TestCase):
    def views(self):
        return {
            "struct A": {"leaves": [leaf(0, "object"), leaf(4, "field4"), leaf(8, "unknown8"),
                                    leaf(0xC, "count", signed=True, ctype="s32")]},
            "struct B": {"leaves": [leaf(0, "object"), leaf(4, "timer", 2, ctype="u16"),
                                    leaf(6, "mode", 2, ctype="u16"), leaf(0xC, "count")]},
        }

    def test_disagreements(self):
        rows = {off: notes for off, _, notes in struct_views.compare(self.views())}
        self.assertEqual(rows[0], [])
        self.assertEqual(rows[4], ["width", "name"])
        self.assertEqual(rows[6], ["inside A.field4"])
        self.assertEqual(rows[0xC], ["signedness"])
        self.assertNotIn(8, rows)  # pad members say nothing

    def test_range(self):
        offs = [off for off, _, _ in struct_views.compare(self.views(), 4, 0xC)]
        self.assertEqual(offs, [4, 6])

    def test_is_pad(self):
        self.assertTrue(struct_views.is_pad("unknown9[0x47]"))
        self.assertTrue(struct_views.is_pad("body.unk4"))
        self.assertFalse(struct_views.is_pad("field9F"))


if __name__ == "__main__":
    unittest.main()
