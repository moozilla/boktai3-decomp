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

    def explain_self_occupied_word(self, want, got):
        # Synthetic doubleword: the global conflict summary includes the pair
        # that local allocation already assigned to this same pseudo.
        rows = [(10, "insn", "(clobber (reg:DI 43))"),
                (11, "insn", "(set (subreg:SI (reg:DI 43) 0) (reg:SI 50))"),
                (12, "insn", "(set (reg:SI 0 r0) (subreg:SI (reg:DI 43) 1))")]
        recs = {43: ("L", 1, 20000, 2, 8, 6, 2)}
        with tempfile.TemporaryDirectory() as wd, \
                mock.patch.object(self.scope, "alloc_records", return_value=recs), \
                mock.patch.object(self.scope, "conflicts", return_value={43: ({43}, {2, 3})}), \
                mock.patch.object(self.scope, "hard_of", return_value={43: 2}), \
                mock.patch.object(self.scope, "insn_list", return_value=rows):
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                self.scope.explain_alloc(wd, "F", 43, want, got, str, str)
        return out.getvalue()

    def test_own_upper_word_is_not_an_unknown_holder(self):
        output = self.explain_self_occupied_word(want=3, got=2)
        self.assertIn("2-word local-alloc value", output)
        self.assertIn("starting at r2 and spanning {r2, r3}", output)
        self.assertIn("word 0 in r2", output)
        self.assertIn("r3 already holds word 1 of 43 itself", output)
        self.assertIn("allocation start r3 and span {r3, r4}", output)
        self.assertIn("AGBCC_FORCE=43:3 (moves the whole allocation)", output)
        self.assertNotIn("holder not found", output)
        self.assertNotIn("shorten", output)

    def test_high_word_request_moves_the_allocation_start(self):
        output = self.explain_self_occupied_word(want=2, got=3)
        self.assertIn("word 1 in r3", output)
        self.assertIn("allocation start r1 and span {r1, r2}", output)
        self.assertIn("AGBCC_FORCE=43:1 (moves the whole allocation)", output)
        self.assertNotIn("evict", output)
        self.assertNotIn("holder not found", output)

    def test_high_word_operand_retains_the_allocation_base(self):
        before = "(set (reg:SI 0 r0) (subreg:SI (reg/v:DI 43) 1))"
        after = "(set (reg:SI 0 r0) (reg:SI 3 r3))"
        dumps = {"lreg": [(10, "insn", before)], "greg": [(10, "insn", after)]}
        with tempfile.TemporaryDirectory() as wd, \
                mock.patch.object(self.scope, "insn_list", side_effect=lambda _, p: dumps[p]):
            hard = self.scope.hard_of(wd)
        self.assertEqual(hard, {43: 2})
        self.assertEqual(self.scope.register_words(before), [(0, 0), (43, 1)])
        self.assertEqual([reg for reg, word in self.scope.register_words(before)
                          if hard.get(reg, reg) + word == 3], [43])


if __name__ == "__main__":
    unittest.main()
