"""`life` names counted insns that combine deletes, and the halfword field insertion behind them."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class CombineDead(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_halfword_zero_is_named(self):
        flow = {10: "(set (reg:SI 30) (reg:SI 22))",
                11: "(set (reg:HI 34) (const_int 0))",
                12: "(set (reg:SI 39) (ior:SI (reg:SI 29) (subreg:SI (reg:HI 34) 0)))"}
        note = self.scope.combine_dead_note([10, 11, 12], {10, 12}, flow, {10: 5, 11: 96, 12: 96})
        self.assertIn("line 96 (1)", note)
        self.assertNotIn("line 5", note)
        self.assertIn("fact 34", note)

    def test_other_deleted_insns_are_not_reported(self):
        flow = {10: "(set (reg:SI 30) (reg:SI 22))"}
        self.assertEqual(self.scope.combine_dead_note([10], set([11]), flow, {10: 5}), "")

    def test_no_combine_dump(self):
        self.assertEqual(self.scope.combine_dead_note([10], set(), {10: "x"}, {}), "")


if __name__ == "__main__":
    unittest.main()
