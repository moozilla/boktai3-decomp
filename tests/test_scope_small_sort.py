"""`scope.py` explains local-alloc's hand sort of exactly three quantities (fact 37)."""
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope

# PlayerCue_UpdateFacingPose's then-branch with `p->facing = (8 - p->dir) >> 1;`: dir, the
# 8 / difference / shift chain and the facing address, in qty-number order.
BLOCK4 = """L sub_08241714 3 40 0 17500 4 12 7 0 0 value
L sub_08241714 4 53 0 5000 2 6 2 0 0 -
L sub_08241714 4 56 1 15000 4 12 6 0 1 -
L sub_08241714 4 59 2 10000 10 12 2 0 0 -
"""


class SmallSort(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_three_qtys_compare_qty_numbers(self):
        # qty 1 beats qty 0 and qty 2 does not beat qty 1: the two swaps of 0 and 1 cancel
        used, real = self.scope.small_sort_order([5000, 15000, 10000])
        self.assertEqual(used, [0, 1, 2])
        self.assertEqual(real, [1, 2, 0])

    def test_rising_priorities_reverse(self):
        used, real = self.scope.small_sort_order([1, 2, 3])
        self.assertEqual(used, [2, 1, 0])
        self.assertEqual(real, [2, 1, 0])

    def test_two_and_four_qtys_sort_by_priority(self):
        self.assertEqual(self.scope.small_sort_order([1, 2])[0], [1, 0])
        self.assertEqual(self.scope.small_sort_order([1, 4, 3, 2])[0], [1, 2, 3, 0])

    def test_note_names_the_order(self):
        with tempfile.TemporaryDirectory() as wd:
            Path(wd, "names.txt").write_text(BLOCK4)
            note = self.scope.small_sort_note(wd, "sub_08241714", 4)
            self.assertIn("exactly three local quantities", note)
            self.assertIn("allocated as 53 (priority 5000), 56 (priority 15000), 59", note)
            self.assertIsNone(self.scope.small_sort_note(wd, "sub_08241714", 3))


if __name__ == "__main__":
    unittest.main()
