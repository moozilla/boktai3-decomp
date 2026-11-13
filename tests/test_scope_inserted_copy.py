"""Do not attribute a new two-address copy to a later, unrelated tie."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope


class InsertedOperandCopy(unittest.TestCase):
    def setUp(self):
        self.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def fixture(self, wd, *, existing=False, boundary=False, wrong_source=False):
        copy = "(insn 201 20 27 (set (reg/v:SI 30) (reg/v:SI %d)) -1 (nil) (nil))\n" % (
            29 if wrong_source else 26)
        before = "(insn 27 20 29 (set (reg/v:SI 30) (and:SI (reg/v:SI 26) (reg/v:SI 29))) 36 {andsi3} (nil) (nil))\n"
        after = before.replace("(and:SI (reg/v:SI 26)", "(and:SI (reg/v:SI 30)")
        later = "(insn 52 50 54 (set (reg/v:SI 31) (and:SI (reg/v:SI 30) (reg/v:SI 26))) 36 {andsi3} (nil) (nil))\n"
        Path(wd, "u.i.combine").write_text((copy if existing else "") + before + later)
        Path(wd, "u.i.regmove").write_text(
            "Fixed operand 2 of insn 52 matching operand 0.\n" + copy
            + ('(code_label 202 201 27 1 "")\n' if boundary else "") + after + later)

    def test_inserted_copy_takes_precedence_over_later_tie(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd)
            self.assertEqual(self.scope.regmove_inserted_operand_copy(wd, 27), (201, 30, 26))
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                self.scope.explain_regmove(wd, 27, [26], [], str, lambda u: f"insn {u}")
            self.assertIn("regmove inserted 30 = 26", out.getvalue())
            self.assertNotIn("insn 52 is a two-address", out.getvalue())
            self.assertNotIn("what-if: AGBCC_NOTIE=52", out.getvalue())

    def test_existing_copy_is_not_new_operand_copy(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd, existing=True)
            self.assertIsNone(self.scope.regmove_inserted_operand_copy(wd, 27))

    def test_copy_across_label_is_not_local_evidence(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd, boundary=True)
            self.assertIsNone(self.scope.regmove_inserted_operand_copy(wd, 27))

    def test_copy_must_supply_changed_operand(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd, wrong_source=True)
            self.assertIsNone(self.scope.regmove_inserted_operand_copy(wd, 27))
