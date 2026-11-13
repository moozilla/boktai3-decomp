"""A shared mask is not evidence that a distant regmove tie caused a change."""
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path
from unittest.mock import patch
import unittest

from tools.matchsearch.evaluator import _load_scope


class RegmoveAttribution(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def explain(self, ties):
        before = [(64, 'insn', '(set (reg:SI 47) (and:SI (reg:SI 35) (reg:SI 46)))'),
                  (391, 'insn', '(set (reg:SI 90) (and:SI (reg:SI 35) (reg:SI 89)))')]
        after = [(64, 'insn', '(set (reg:SI 35) (and:SI (reg:SI 35) (reg:SI 46)))')]
        out = StringIO()
        with patch.object(self.scope, 'regmove_fixes', return_value=ties), \
             patch.object(self.scope, 'insn_list', side_effect=lambda wd, stage: before if stage == 'combine' else after), \
             patch.object(self.scope, 'reg_copy_coalesce', return_value=None), redirect_stdout(out):
            self.scope.explain_regmove('missing-test-directory', 64, [47], [35], str, str)
        return out.getvalue()

    def test_shared_input_does_not_name_unrelated_tie(self):
        note = self.explain([('tie', 391, 1)])
        self.assertNotIn('AGBCC_NOTIE=391', note)
        self.assertIn('no tie', note)

    def test_exact_tie_still_wins(self):
        note = self.explain([('tie', 391, 1), ('tie', 64, 1)])
        self.assertIn('AGBCC_NOTIE=64', note)
        self.assertNotIn('AGBCC_NOTIE=391', note)


if __name__ == '__main__':
    unittest.main()
