"""Explain unused spill slots without mistaking executable operands for USEs."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class UseOnlyPseudos(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_deleted_input_survives_as_use(self):
        before = [(224, 'insn', '(set (reg:SI 95) (and:SI (reg/v:SI 94) (const_int -65536)))')]
        after = [(939, 'insn', '(use (reg/v:SI 94))')]
        self.assertEqual(self.scope.use_only_pseudos(before, after), {94: [939]})

    def test_surviving_executable_mentions_disqualify(self):
        before = [(1, 'insn', '(set (reg:SI 95) (reg:SI 94))')]
        for operation in ('(set (reg:SI 94) (const_int 0))',
                          '(set (reg:SI 96) (reg:SI 94))',
                          '(clobber (reg:SI 94))',
                          '(use (subreg:QI (reg:SI 94) 0))'):
            with self.subTest(operation=operation):
                after = [(2, 'insn', '(use (reg:SI 94))'), (3, 'insn', operation)]
                self.assertEqual(self.scope.use_only_pseudos(before, after), {})

    def test_absent_or_already_use_only_before_is_not_evidence(self):
        after = [(2, 'insn', '(use (reg:SI 94))')]
        self.assertEqual(self.scope.use_only_pseudos([], after), {})
        self.assertEqual(self.scope.use_only_pseudos(after, after), {})
        self.assertEqual(self.scope.use_only_pseudos(after, []), {})

    def test_multiple_uses_and_exact_pseudo_numbers(self):
        before = [(1, 'insn', '(set (reg:SI 95) (reg:SI 94))')]
        after = [(2, 'insn', '(use (reg:SI 94))'),
                 (3, 'insn', '(use (reg:SI 94))'),
                 (4, 'insn', '(set (reg:SI 194) (const_int 0))')]
        self.assertEqual(self.scope.use_only_pseudos(before, after), {94: [2, 3]})

    def test_hard_register_uses_are_not_spill_candidates(self):
        before = [(1, 'insn', '(set (reg:SI 95) (reg:SI 0))')]
        after = [(2, 'insn', '(use (reg:SI 0))')]
        self.assertEqual(self.scope.use_only_pseudos(before, after), {})


if __name__ == '__main__':
    unittest.main()
