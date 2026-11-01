"""Landing refuses a match that adds register pins or asm barriers unless asked with --allow-pins;
pins inside #ifdef NONMATCHING drafts don't count, since the default build never compiles them."""
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "tools", "scope"))
import land  # noqa: E402

PINNED = 'int F(void) { register int x asm("r4"); asm("" : "+r"(x)); return x; }\n'
DRAFT = '#ifdef NONMATCHING\n' + PINNED + '#else\nINCLUDE_ASM("asm/nonmatching", sub_08000000);\n#endif\n'
KEPT = 'int G(void) { register int y asm("r5"); return y; }\n'


class LandPins(unittest.TestCase):
    def test_draft_pins_are_not_compiled(self):
        self.assertNotIn("asm(\"r4\")", land.without_drafts(KEPT + DRAFT))
        self.assertIn("asm(\"r5\")", land.without_drafts(KEPT + DRAFT))

    def test_a_pinned_match_of_a_pinned_draft_adds_pins(self):
        self.assertEqual(land.added_concessions(KEPT + DRAFT, KEPT + PINNED), (1, 1))

    def test_a_pin_free_match_adds_none(self):
        self.assertEqual(land.added_concessions(KEPT + DRAFT, KEPT + "int F(void) { return 0; }\n"), (0, 0))

    def test_removing_pins_elsewhere_adds_none(self):
        self.assertEqual(land.added_concessions(KEPT + DRAFT, "int G(void) { return 0; }\n" + DRAFT), (0, 0))

    def test_an_empty_do_while_counts_as_a_barrier(self):
        loop = "int F(void) { do {\n    } while (0);\n    return 0; }\n"
        self.assertEqual(land.added_concessions(DRAFT, loop), (0, 1))
        self.assertEqual(land.added_concessions(DRAFT, "int F(void) { do { g(); } while (0); return 0; }\n"), (0, 0))

    def test_an_empty_if_counts_unless_its_body_is_debug_print(self):
        self.assertEqual(land.added_concessions(DRAFT, "int F(void) { if (G(1)) { } return 0; }\n"), (0, 1))
        self.assertEqual(land.added_concessions(DRAFT, "int F(void) { if (G(1)) {\n DEBUG_PRINT();\n }\n"
                                                       " return 0; }\n"), (0, 0))

    def test_refusal_names_the_flag_and_the_pins_command(self):
        text = land.pin_refusal("F", 1, 0, "src/fn/f.c")
        self.assertIn("1 register pin.", text)
        self.assertIn("--allow-pins", text)
        self.assertIn("scope.py src/fn/f.c pins", text)


if __name__ == "__main__":
    unittest.main()
