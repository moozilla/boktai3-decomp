"""tools/fold_literals.py: which single-use literal locals get written in place, and the edit."""
import os
import sys
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import fold_literals as fl  # noqa: E402


def fold(body, params="u32 *flags, s32 a"):
    text = f"void F({params})\n{{\n{body}}}\n"
    return [new for new, _ in fl.candidates(text, "src/x.c")], text


class Fold(unittest.TestCase):
    def test_single_use_is_folded(self):
        news, _ = fold("    u32 mask = 0x40;\n    if (*flags & mask)\n        a = 1;\n")
        self.assertEqual(news, ["void F(u32 *flags, s32 a)\n{\n    if (*flags & 0x40)\n        a = 1;\n}\n"])

    def test_two_uses_keep_the_name(self):
        news, _ = fold("    u32 mask = 0x40;\n    *flags |= mask;\n    *flags ^= mask;\n")
        self.assertEqual(news, [])

    def test_separate_assignment(self):
        news, _ = fold("    u32 kind;\n    a = 2;\n    kind = 7;\n    Call(kind);\n")
        self.assertEqual(news, ["void F(u32 *flags, s32 a)\n{\n    a = 2;\n    Call(7);\n}\n"])

    def test_value_must_fit_the_type(self):
        self.assertEqual(fold("    u8 v = 0x1FF;\n    Call(v);\n")[0], [])

    def test_negative_unsigned_only_as_a_bitwise_operand(self):
        self.assertEqual(len(fold("    u32 mask = ~3;\n    *flags &= mask;\n")[0]), 1)
        self.assertEqual(fold("    u32 m = -1;\n    if (a == m)\n        a = 0;\n")[0], [])

    def test_sizeof_and_address_are_left(self):
        self.assertEqual(fold("    u16 v = 3;\n    a = sizeof(v);\n")[0], [])
        self.assertEqual(fold("    u16 v = 3;\n    Call(&v);\n")[0], [])

    def test_shadowed_name_is_left(self):
        body = "    u32 m = 1;\n    a = m;\n    {\n        u32 m = 2;\n        a += m;\n    }\n"
        self.assertEqual(fold(body)[0], [])

    def test_negative_after_minus_is_left(self):
        self.assertEqual(fold("    s32 d = -4;\n    a = a - d;\n")[0], [])

    def test_drafts_are_left(self):
        text = "#ifdef NONMATCHING\nvoid F(u32 *flags)\n{\n    u32 mask = 0x40;\n    *flags |= mask;\n}\n" \
               "#else\nINCLUDE_ASM(\"asm/nonmatching\", F);\n#endif\n"
        self.assertEqual(fl.candidates(text, "src/x.c"), [])

    def test_a_fold_that_adds_a_raw_offset_is_refused(self):
        news, text = fold("    u32 off = 0x8D0;\n    a = *(u16 *)((u8 *)flags + off);\n")
        self.assertEqual(len(news), 1)
        self.assertTrue(fl.adds_concession(text, news[0], "src/x.c"))
        news, text = fold("    u32 mask = 0x40;\n    *flags |= mask;\n")
        self.assertFalse(fl.adds_concession(text, news[0], "src/x.c"))

    def test_member_of_the_same_name_is_not_a_read(self):
        news, _ = fold("    u32 state = 3;\n    p->state = 1;\n    Call(state);\n", "struct S *p")
        self.assertEqual(news, ["void F(struct S *p)\n{\n    p->state = 1;\n    Call(3);\n}\n"])


if __name__ == "__main__":
    unittest.main()
