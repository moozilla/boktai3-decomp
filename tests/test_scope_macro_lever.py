"""`why` suggests the statement-macro (do-while(0)) lever for a global-alloc priority swap."""
from pathlib import Path
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

    def test_hint_requires_a_shared_macro(self):
        s = self.scope
        saved = s.macro_lever, s.mention_lines
        try:
            s.macro_lever = lambda *a, **k: (54, 59, 16, 10)
            s.mention_lines = lambda *a, **k: ""
            out = s.flip_levers(str, 26, ("G", 87, 2748, 12, 131, 0, 7), 77, ("G", 81, 3947, 10, 76, 0, 5), wd="w")
        finally:
            s.macro_lever, s.mention_lines = saved
        hint = next(o for o in out if o.startswith("a statement macro"))
        self.assertIn("several functions share", hint)
        self.assertIn("single-use wrapper is a priority knob and counts as a barrier", hint)


if __name__ == "__main__":
    unittest.main()
