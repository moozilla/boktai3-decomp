"""`land` puts a rewrite of a function that matches already (a phase-3 depin) back into the unit that
defines it, instead of a new src/fn file: land.where falls back to the defining unit."""
import os
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "tools", "scope"))
import land  # noqa: E402

UNIT = ('#include "global.h"\n\nvoid A(void)\n{\n}\n\n// Comment above B.\n'
        '// MATCHING_CONCESSION: register-pinning\nvoid B(int x)\n{\n    register int y asm("r4") = x;\n}\n\n'
        'INCLUDE_ASM("asm/nonmatching", sub_08000100);\n')
DRAFTED = '#ifdef NONMATCHING\nvoid C(void)\n{\n}\n#else\nINCLUDE_ASM("asm/nonmatching", C);\n#endif\n'


class LandMatched(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = self.tmp.name
        os.makedirs(os.path.join(self.root, "src", "fn"))
        open(os.path.join(self.root, "src", "unit.c"), "w").write(UNIT)
        open(os.path.join(self.root, "src", "fn", "draft.c"), "w").write(DRAFTED)

    def tearDown(self):
        self.tmp.cleanup()

    def test_a_matched_function_lands_in_its_unit(self):
        self.assertEqual(land.where(self.root, "B", 0x08000080), ("unit", "src/unit.c", None))

    def test_existing_rom_label_alias_is_the_definition(self):
        alias = "#define Weather_Init sub_08018028\n"
        proto = "int Weather_Init(int);\n"
        definition = "int Weather_Init(int x)\n{\n    return x;\n}\n"
        text = alias + proto + definition
        with open(os.path.join(self.root, "src", "weather.c"), "w") as out:
            out.write(text)
        self.assertEqual(land.where(self.root, "sub_08018028", 0x08018028),
                         ("unit", "src/weather.c", None))
        self.assertEqual(land.without_definition(text, "sub_08018028"), alias + proto)

    def test_an_include_asm_still_wins(self):
        self.assertEqual(land.where(self.root, "sub_08000100", 0x08000100),
                         ("unit", "src/unit.c", "sub_08000100"))

    def test_a_nonmatching_draft_is_not_a_definition(self):
        self.assertIsNone(land.defining_unit(self.root, ["C"]))
        self.assertEqual(land.where(self.root, "C", 0x08000200), ("unit", "src/fn/draft.c", "C"))

    def test_an_unknown_function_is_plain(self):
        self.assertEqual(land.where(self.root, "D", 0x08000300), ("plain", "src/fn/sub_08000300.c", None))

    def test_without_definition_cuts_the_comments_above(self):
        rest = land.without_definition(UNIT, "B")
        self.assertNotIn("Comment above B", rest)
        self.assertNotIn("MATCHING_CONCESSION", rest)
        self.assertIn("void A(void)", rest)
        self.assertIn("INCLUDE_ASM", rest)

    def test_only_additions_counts_a_new_helper(self):
        helper = "static inline int H(void)\n{\n    return 1;\n}\n\n"
        self.assertEqual(land.only_additions(UNIT, helper + UNIT), 4)
        self.assertEqual(land.only_additions(UNIT, UNIT.replace("    ", "\t")), 0)

    def test_only_additions_refuses_a_changed_line(self):
        self.assertIsNone(land.only_additions(UNIT, UNIT.replace("void A(void)", "void A(int)")))
        self.assertIsNone(land.only_additions(UNIT, UNIT.replace('#include "global.h"\n', "")))


if __name__ == "__main__":
    unittest.main()
