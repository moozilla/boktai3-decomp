"""fold_views places a private view's members into the class: rename, adopt or insert."""
import os
import sys
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "tools"))

import fold_views as fv  # noqa: E402


def leaf(name, off, size, signed=False, array=False):
    return {"name": name, "off": off, "size": size, "width": 1 if array else size, "signed": signed,
            "base": "int", "bits": None, "array": array, "ctype": "u8"}


CLASS = {"size": 0x20, "leaves": [
    leaf("chase", 0, 8, array=True),       # the embedded base
    leaf("field8", 8, 2),
    leaf("unknownA", 0xA, 0xE, array=True),
    leaf("flags18", 0x18, 2),
]}


class Plan(unittest.TestCase):
    def plan(self, leaves, decls):
        return fv.plan(CLASS, {"size": 0x20, "leaves": leaves}, decls, 8)[:4]

    def test_insert_into_padding(self):
        renames, adopted, inserts, problems = self.plan(
            [leaf("unknown0", 0, 0xC, array=True), leaf("speed", 0xC, 4)], {"speed": "u32 speed"})
        self.assertEqual((renames, adopted, problems), ({}, {}, []))
        self.assertEqual([(p["name"], d) for p, _, d in inserts], [("unknownA", "u32 speed")])

    def test_reader_name_replaces_offset_name(self):
        _, adopted, _, _ = self.plan([leaf("cooldown", 8, 2)], {})
        self.assertEqual(adopted, {"field8": "cooldown"})

    def test_weak_name_takes_class_name(self):
        renames, adopted, _, _ = self.plan([leaf("flags", 0x18, 2)], {})
        self.assertEqual((renames, adopted), ({"flags": "flags18"}, {}))

    def test_placeholder_inserted_under_offset_name(self):
        renames, _, inserts, _ = self.plan([leaf("b", 0x10, 2)], {"b": "s16 b"})
        self.assertEqual(renames, {"b": "field10"})
        self.assertEqual(inserts[0][2], "s16 field10")

    def test_conflicts_reported(self):
        _, _, _, problems = self.plan([leaf("x", 2, 2), leaf("wide", 8, 4)], {})
        self.assertEqual(len(problems), 2)  # inside the base; across field8 and the padding


class Members(unittest.TestCase):
    def test_array_of_structs_ends_at_next_member(self):
        # the probe lists only element 0 of `struct C colliders[3]` (0x54 bytes each)
        ms = fv.members([leaf("colliders[0].next", 0x10, 4), leaf("colliders[0].owner", 0x60, 4),
                         leaf("after", 0x10 + 3 * 0x54, 2)], 0x200)
        self.assertEqual((ms[0]["name"], ms[0]["end"]), ("colliders", 0x10 + 3 * 0x54))


class Text(unittest.TestCase):
    def test_split_padding(self):
        text = "struct C {\n    u8 chase[8];\n    u16 field8;\n    u8 unknownA[0xE];\n    u16 flags18;\n};\n"
        pad = {"name": "unknownA", "off": 0xA, "end": 0x18}
        out = fv.apply_inserts(text, "C", [(pad, {"off": 0xC, "end": 0x10}, "u32 speed")])
        self.assertIn("    u8 unknownA[2];\n    u32 speed;\n    u8 unknown10[8];\n    u16 flags18;", out)

    def test_declarations(self):
        d = fv.declarations("    u8 unknown0[0x6C4];\n    u32 requests, nearDistance;\n    void *p; /* c */\n")
        self.assertEqual(d["nearDistance"], "u32 nearDistance")
        self.assertEqual(d["p"], "void *p")

    def test_unit_edit(self):
        text = '#include "global.h"\nstruct V {\n    u8 unknown0[8];\n};\nvoid f(struct V *v);\n'
        out = fv.edit_unit(text, "V", "Vorn", "include/vorn.h")
        self.assertEqual(out, '#include "global.h"\n#include "vorn.h"\nvoid f(struct Vorn *v);\n')


if __name__ == "__main__":
    unittest.main()
