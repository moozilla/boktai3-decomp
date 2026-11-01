"""`why` suggests the statement-macro (do-while(0)) lever for a global-alloc priority swap."""
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope


class MacroLever(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_narrowest_run_that_flips_the_order(self):
        # zDelta: 12 refs over 131 insns (2748); the mode bit: 10 refs over 76 (3947).
        lo_rec = ("G", 87, 2748, 12, 131, 0, 7)
        hi_rec = ("G", 81, 3947, 10, 76, 0, 5)
        mentions = {26: [(95, 1), (100, 1), (101, 1), (110, 1), (124, 1), (127, 1), (227, 1), (301, 1)],
                    77: [(145, 1), (146, 1)]}
        lines = {95: 54, 100: 56, 101: 56, 110: 59, 124: 64, 127: 65, 227: 77, 301: 90, 145: 66, 146: 66}
        a, b, nl, nh = self.scope.macro_lever(None, 26, lo_rec, 77, hi_rec, mentions, lines)
        self.assertEqual((nl, nh), (16, 10))
        self.assertEqual((a, b), (54, 59))
        self.assertGreater(self.scope.g_priority(nl, 131), self.scope.g_priority(nh, 76))

    def test_rival_mentions_inside_the_run_count_too(self):
        lo_rec = ("G", 2, 1000, 4, 80, 0, 7)
        hi_rec = ("G", 1, 1250, 4, 64, 0, 5)
        # Every line that holds the low value's refs also holds the rival's: no run helps.
        mentions = {1: [(10, 1), (11, 1)], 2: [(12, 1), (13, 1)]}
        lines = {10: 5, 11: 6, 12: 5, 13: 6}
        self.assertIsNone(self.scope.macro_lever(None, 1, lo_rec, 2, hi_rec, mentions, lines))

    def test_each_line_that_flips_alone(self):
        # sub_080A34BC: the table (3 refs, life 100) against the 0x3FF mask (3 refs, life 86).
        lo_rec = ("G", 257, 300, 3, 100, 0, 9)
        hi_rec = ("G", 263, 348, 3, 86, 0, 12)
        mentions = {257: [(494, 1), (510, 1), (900, 1)], 263: [(500, 1), (503, 1), (896, 1)]}
        lines = {494: 28, 510: 30, 900: 30, 500: 29, 503: 29, 896: 29}
        self.assertEqual(self.scope.macro_lines(None, 257, lo_rec, 263, hi_rec, mentions, lines), [28, 30])


class SharedMacros(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_shared_through_an_inline_and_not_single_use(self):
        with tempfile.TemporaryDirectory() as root:
            Path(root, "include").mkdir()
            Path(root, "src", "fn").mkdir(parents=True)
            Path(root, "include", "r.h").write_text(
                "#define READ_SAMPLE(out, table, index) \\\n    do { \\\n        (out) = (table)[(index)]; \\\n"
                "    } while (0)\n"
                "#define ONCE(x) \\\n    do { \\\n        (x) = 1; \\\n    } while (0)\n"
                "#define NOTHING() \\\n    do { \\\n    } while (0)\n"
                "static inline unsigned ReadSample(const unsigned short *t)\n{\n    unsigned s;\n"
                "    READ_SAMPLE(s, t, 3);\n    return s;\n}\n")
            Path(root, "src", "fn", "a.c").write_text("x = ReadSample(t); ONCE(y); NOTHING();\n")
            Path(root, "src", "fn", "b.c").write_text("READ_SAMPLE(v, t, i); NOTHING();\n")
            rows = self.scope.shared_statement_macros(root=root, limit=5)
            self.assertEqual([r[:3] for r in rows], [("READ_SAMPLE", "include/r.h", 2)])
            self.assertEqual(self.scope.shared_statement_macros(want_index=True, root=root)[0][0], "READ_SAMPLE")


if __name__ == "__main__":
    unittest.main()
