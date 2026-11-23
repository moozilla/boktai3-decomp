"""tools/rawfield.py: finding raw byte-offset accesses, the base's declared type, the members at an
offset, and the access that replaces each form (layouts and the keep gate come from the compiler)."""
import os
import sys
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import rawfield as rf  # noqa: E402


def sites(code):
    return [(s.form, s.ctype, s.amp, s.var, s.off, s.field) for s in rf.find_sites(code)]


LEAVES = [
    {"name": "flags", "off": 0, "size": 4, "width": 4, "signed": False, "base": "int", "array": False, "bits": None},
    {"name": "pos.x", "off": 4, "size": 2, "width": 2, "signed": True, "base": "int", "array": False, "bits": None},
    {"name": "pos.y", "off": 6, "size": 2, "width": 2, "signed": True, "base": "int", "array": False, "bits": None},
    {"name": "shadow", "off": 8, "size": 8, "width": 1, "signed": False, "base": "int", "array": True, "esize": 1,
     "bits": None},
    {"name": "items[0].a", "off": 16, "size": 4, "width": 4, "signed": True, "base": "int", "array": False,
     "bits": None},
    {"name": "cb", "off": 52, "size": 4, "width": 4, "signed": False, "base": "ptr", "array": False, "bits": None},
    {"name": "bit", "off": 56, "size": 1, "width": 1, "signed": False, "base": "int", "array": False, "bits": 1},
]


class Find(unittest.TestCase):
    def test_forms(self):
        self.assertEqual(sites("x = *(u16 *)((u8 *)actor + 0x2D0);"), [("deref", "u16", False, "actor", 0x2D0, None)])
        self.assertEqual(sites("x = *((u8 *)p + 4);"), [("deref8", "u8", False, "p", 4, None)])
        self.assertEqual(sites("q = (struct Seq *)((u8 *)p + 0x48);"),
                         [("addr", "struct Seq", False, "p", 0x48, None)])
        self.assertEqual(sites("((struct W *)((u8 *)s + 16))->flags &= ~1;"),
                         [("arrow", "struct W", False, "s", 16, "flags")])
        self.assertEqual(sites("f((u8 *)owner + 0x120, 2);"), [("bare", "u8", False, "owner", 0x120, None)])
        self.assertEqual(sites("u8 *r = (u8 *)&gState + 8;"), [("bare", "u8", True, "gState", 8, None)])
        self.assertEqual(sites("cb = *(void **)((u8 *)p + 52);"), [("deref", "void *", False, "p", 52, None)])

    def test_not_a_site(self):
        self.assertEqual(sites("r = (u8 *)p + 0xf4 + i * 16;"), [])  # more arithmetic follows
        self.assertEqual(sites("q = (u16 *)((u8 *)p + 4)[i];"), [])  # &p->m[i] would bind otherwise
        self.assertEqual(sites("x = *(u16 *)((u8 *)p->q + 4);"), [])  # the base is not a variable
        self.assertEqual(sites("x = *(u16 *)((u8 *)p + off);"), [])  # a varying offset is ordinary C


class Types(unittest.TestCase):
    def test_parameter_and_local(self):
        code = "void f(struct Actor *actor, s32 n)\n{\n    struct Foo *a = 0, *p;\n    u8 *b;\n    X;\n}\n"
        pos = code.index("X;")
        self.assertEqual(rf.declared_type(code, 0, pos, "actor"), ("struct Actor", True))
        self.assertEqual(rf.declared_type(code, 0, pos, "p"), ("struct Foo", True))
        self.assertIsNone(rf.declared_type(code, 0, pos, "b"))
        self.assertTrue(rf.declared_type_scalar(code, 0, pos, "b"))

    def test_no_match_inside_a_longer_name(self):
        code = "void f(void)\n{\n    pp = 1;\n    X;\n}\n"
        self.assertIsNone(rf.declared_type(code, 0, code.index("X;"), "p"))

    def test_typedef_and_global(self):
        code = "void f(Actor *self)\n{\n    X;\n}\n"
        self.assertEqual(rf.declared_type(code, 0, code.index("X;"), "self"), ("Actor", True))
        pre = "extern struct GameState *gGameState;\nextern struct Clock gClock;\nextern u8 gBytes;\n"
        self.assertEqual(rf.global_type(pre, "gGameState"), ("struct GameState", True))
        self.assertEqual(rf.global_type(pre, "gClock"), ("struct Clock", False))
        self.assertIsNone(rf.global_type(pre, "gBytes"))


class Members(unittest.TestCase):
    def test_candidates_outermost_first(self):
        self.assertEqual([p for p, _ in rf.candidates(LEAVES, 4)], ["pos", "pos.x"])
        self.assertEqual([p for p, _ in rf.candidates(LEAVES, 16)], ["items[0]", "items[0].a"])
        self.assertEqual(rf.candidates(LEAVES, 11), [])  # a byte buffer's element: an offset in disguise
        halves = [{"name": "inventory", "off": 0, "size": 16, "width": 2, "signed": True, "base": "int",
                   "array": True, "esize": 2, "bits": None}]
        self.assertEqual([p for p, _ in rf.candidates(halves, 6)], ["inventory[3]"])
        self.assertEqual([p for p, _ in rf.candidates(LEAVES, 6)], ["pos.y"])
        self.assertEqual(rf.candidates(LEAVES, 5), [])
        self.assertEqual([p for p, _ in rf.candidates(LEAVES, 56)], [])  # a bitfield has no address

    def test_padding_arrays_and_byte_buffers_do_not_count(self):
        leaves = [{"name": "pad10", "off": 0, "size": 16, "width": 1, "signed": False, "base": "int", "array": True,
                   "esize": 1, "bits": None},
                  {"name": "field10", "off": 16, "size": 2, "width": 2, "signed": False, "base": "int",
                   "array": False, "bits": None}]
        self.assertEqual(rf.candidates(leaves, 0), [])
        self.assertEqual(rf.candidates(leaves, 4), [])
        self.assertEqual([p for p, _ in rf.candidates(leaves, 16)], ["field10"])

    def test_scalar_width_and_signedness(self):
        x = dict(LEAVES[1])
        self.assertTrue(rf.scalar_ok("s16", x))
        self.assertFalse(rf.scalar_ok("u16", x))
        self.assertFalse(rf.scalar_ok("s32", x))
        self.assertTrue(rf.scalar_ok("u8", LEAVES[3]))
        self.assertFalse(rf.scalar_ok("u32", LEAVES[5]))  # a pointer member

    def test_spelling(self):
        def s(form, amp=False, field=None):
            return rf.Site(0, 0, form, "u8", amp, "p", 0, field)
        self.assertEqual(rf.spelled(s("deref"), "pos.x"), "p->pos.x")
        self.assertEqual(rf.spelled(s("addr"), "pos"), "&p->pos")
        self.assertEqual(rf.spelled(s("bare"), "shadow[0]"), "p->shadow")
        self.assertEqual(rf.spelled(s("bare", amp=True), "pos"), "&p.pos")
        self.assertEqual(rf.spelled(s("arrow", field="flags"), "w"), "p->w.flags")


if __name__ == "__main__":
    unittest.main()
