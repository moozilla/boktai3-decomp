"""`why`'s priority levers name gcse's shared copy for a variable joined from two paths."""
from pathlib import Path
from unittest import mock
import unittest

from tools.matchsearch.evaluator import _load_scope


class CopyJoined(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def run_with(self, rows, reg):
        with mock.patch.object(self.scope, "insn_list", return_value=rows):
            return self.scope.copy_joined("wd", reg)

    def test_two_sets_one_a_copy(self):
        rows = [(1, 0, "(set (reg/v:SI 23) (plus:SI (reg/v:SI 30) (const_int 192)))"),
                (2, 0, "(set (reg/v:SI 23) (reg/v:SI 41))")]
        self.assertTrue(self.run_with(rows, 23))

    def test_single_set_or_no_copy(self):
        self.assertFalse(self.run_with([(1, 0, "(set (reg/v:SI 23) (reg/v:SI 41))")], 23))
        rows = [(1, 0, "(set (reg/v:SI 23) (const_int 0))"),
                (2, 0, "(set (reg/v:SI 23) (plus:SI (reg/v:SI 23) (const_int 1)))")]
        self.assertFalse(self.run_with(rows, 23))


if __name__ == "__main__":
    unittest.main()
