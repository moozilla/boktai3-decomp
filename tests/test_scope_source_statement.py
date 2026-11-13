"""Inline header line numbers must not select unrelated caller statements."""
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope


class SourceStatement(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.wd = Path(self.tmp.name)
        self.src = self.wd / 'caller.c'
        self.src.write_text('void Target(void) {\n    CALLER_MACRO();\n}\n')
        (self.wd / 'u.i').write_text(
            f'# 1 "{self.src}"\nvoid Target(void) {{\n    expanded_caller();\n}}\n'
            '# 1 "include/step.h" 1\nstatic inline void Step(void) {\n    state->phase = 0;\n}\n')
        (self.wd / 'u.i.rtl').write_text(
            f'(note 1 0 2 ("{self.src}") 2)\n\n'
            '(insn 2 1 3 (set (reg:SI 20) (const_int 1)))\n\n'
            '(note/i 3 2 4 ("include/step.h") 2)\n\n'
            '(insn/i 4 3 5 (set (reg:SI 21) (const_int 0)))\n')

    def statement(self, uid):
        return self.scope.source_statement(str(self.wd), str(self.src), uid)

    def test_header_uses_its_own_text_at_colliding_line_number(self):
        self.assertEqual(self.statement(4), ('2 (include/step.h)', 'state->phase = 0;'))

    def test_main_file_keeps_original_macro_spelling(self):
        self.assertEqual(self.statement(2), ('2', 'CALLER_MACRO();'))

    def test_late_instruction_keeps_header_location(self):
        (self.wd / 'u.i.greg').write_text(
            '(note 3 2 4 ("include/step.h") 2)\n\n'
            '(insn 99 3 5 (set (reg:SI 0) (const_int 0)))\n')
        self.assertEqual(self.statement(99), ('2 (include/step.h)', 'state->phase = 0;'))

    def test_missing_header_text_does_not_fall_back_to_caller(self):
        (self.wd / 'u.i').unlink()
        self.assertEqual(self.statement(4), ('2 (include/step.h)', ''))

    def test_unknown_uid_has_no_invented_source(self):
        self.assertEqual(self.statement(88), (None, ''))

    def test_line_only_api_remains_numeric(self):
        self.assertEqual(self.scope.uid_lines(str(self.wd)), {2: 2, 4: 2})

    def test_why_summary_accepts_header_location(self):
        issues = self.scope.why_all_issues('why insn 4  line 2 (include/step.h): state->phase = 0;\n')
        self.assertEqual(issues[0][3], 4)
        self.assertIn('2 (include/step.h)', issues[0][1])


if __name__ == '__main__':
    unittest.main()
