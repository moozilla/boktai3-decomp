import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools" / "scope"))
import scope  # noqa: E402


class SpillOptionsTest(unittest.TestCase):
    def test_first_choice_with_rotation_can_shift(self):
        # sub_08010C10: the first reload choice (start=-1, rotate=1) takes r1; AGBCC_SPILL=0:1 gives r2.
        x = {"start": -1, "rotate": True, "regs": [1, 2], "st": {1: "A", 2: "A"}}
        self.assertEqual(scope.spill_options(x, 1), [(1, 2)])

    def test_forced_group_and_old_trace_do_not_shift(self):
        x = {"start": -1, "rotate": False, "regs": [1, 2], "st": {1: "A", 2: "A"}}
        self.assertEqual(scope.spill_options(x, 1), [])
        x = {"start": -1, "rotate": None, "regs": [1, 2], "st": {1: "A", 2: "A"}}
        self.assertEqual(scope.spill_options(x, 1), [])

    def test_occupied_register_is_skipped(self):
        x = {"start": 0, "rotate": True, "regs": [1, 2, 3], "st": {1: "L79", 2: "A", 3: "A"}}
        self.assertEqual(scope.spill_options(x, 2), [(1, 3)])


if __name__ == "__main__":
    unittest.main()
