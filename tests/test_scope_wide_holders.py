"""Allocation diagnostics include every hard register of a multiword value."""
import contextlib
import io
from pathlib import Path
import tempfile
from unittest import mock
import unittest

from tools.matchsearch.evaluator import _load_scope


class WideHolders(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_subreg_and_narrow_uses_keep_doubleword_width(self):
        rows = [(1, "insn", "(set (subreg:SI (reg/v:DI 33) 0) (reg:SI 42))"),
                (2, "insn", "(set (reg:SI 0 r0) (reg:SI 33))")]
        widths = self.scope.register_widths(rows)
        self.assertEqual(widths, {33: 2, 42: 1, 0: 1})
        self.assertTrue(self.scope.occupies_hard_register(33, 3, {33: 2}, widths))
        self.assertFalse(self.scope.occupies_hard_register(33, 4, {33: 2}, widths))
        self.assertFalse(self.scope.occupies_hard_register(42, 3, {42: 2}, widths))
        self.assertFalse(self.scope.occupies_hard_register(33, 0, {33: -1}, widths))
        self.assertFalse(self.scope.occupies_hard_register(33, 0, {}, widths))

    def test_fixed_hard_register_pair(self):
        self.assertTrue(self.scope.occupies_hard_register(2, 3, {}, {2: 2}))
        self.assertFalse(self.scope.occupies_hard_register(2, 4, {}, {2: 2}))

    def test_palette_bank_holder_is_named(self):
        # The bank's clobber lies between the last set and use of pseudo 42.
        rows = [(61, "insn", "(set (reg:SI 42) (plus:SI (reg:SI 41) (reg:SI 40)))"),
                (67, "insn", "(clobber (reg/v:DI 33))"),
                (63, "insn", "(set (subreg:SI (reg/v:DI 33) 0) (reg:SI 42))")]
        recs = {42: ("G", 0, 26666, 4, 3, 0, 2),
                33: ("L", 0, 30000, 1, 8, 4, 2)}
        with tempfile.TemporaryDirectory() as wd, \
                mock.patch.object(self.scope, "alloc_records", return_value=recs), \
                mock.patch.object(self.scope, "conflicts", return_value={42: (set(), {3})}), \
                mock.patch.object(self.scope, "hard_of", return_value={33: 2, 42: 2}), \
                mock.patch.object(self.scope, "insn_list", return_value=rows):
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                self.scope.explain_alloc(wd, "F", 42, 3, 2, str, str)
        self.assertIn("33 (from 67)", out.getvalue())
        self.assertNotIn("holder not found", out.getvalue())


if __name__ == "__main__":
    unittest.main()
