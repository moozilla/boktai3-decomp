"""Do not attribute a new two-address copy to a later, unrelated tie."""
import contextlib
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest import mock

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
            self.assertIsNone(self.scope.regmove_operand_copy_operation(wd, 201))

    def test_copy_across_label_is_not_local_evidence(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd, boundary=True)
            self.assertIsNone(self.scope.regmove_inserted_operand_copy(wd, 27))
            self.assertIsNone(self.scope.regmove_operand_copy_operation(wd, 201))

    def test_copy_must_supply_changed_operand(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd, wrong_source=True)
            self.assertIsNone(self.scope.regmove_inserted_operand_copy(wd, 27))
            self.assertIsNone(self.scope.regmove_operand_copy_operation(wd, 201))

    def test_copy_uid_identifies_its_operation_across_notes(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd)
            path = Path(wd, "u.i.regmove")
            path.write_text(path.read_text().replace(
                "(insn 27", '(note 202 201 27 "draft.c" 12)\n(insn 27'))
            self.assertEqual(self.scope.regmove_operand_copy_operation(wd, 201), 27)
            self.assertIsNone(self.scope.regmove_operand_copy_operation(wd, 27))
            self.assertIsNone(self.scope.regmove_operand_copy_operation(wd, 999))

    def test_copy_uid_does_not_cross_an_operation_or_control_boundary(self):
        intervening = [
            '(jump_insn 202 201 27 (set (pc) (label_ref 20)) -1 (nil) (nil))',
            '(call_insn 202 201 27 (call (mem:SI (symbol_ref:SI "Other")) (const_int 0)) -1 (nil) (nil))',
            '(barrier 202 201 27)',
            '(insn 202 201 27 (set (reg:SI 40) (const_int 0)) -1 (nil) (nil))',
        ]
        for boundary in intervening:
            with self.subTest(boundary=boundary), tempfile.TemporaryDirectory() as wd:
                self.fixture(wd)
                path = Path(wd, "u.i.regmove")
                path.write_text(path.read_text().replace("(insn 27", boundary + "\n(insn 27"))
                self.assertIsNone(self.scope.regmove_operand_copy_operation(wd, 201))

    def test_why_copy_reports_operand_cause_without_allocation_advice(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd)
            source = Path(wd, "draft.c")
            source.write_text("void Target(void) {}\n")
            Path(wd, "stderr.txt").write_text("")
            Path(wd, "u.i.lreg").write_text(Path(wd, "u.i.regmove").read_text())
            Path(wd, "u.i.greg").write_text(
                ";; Register dispositions:\n30 in 0 26 in 2\n\n"
                "(insn 201 20 27 (set (reg:SI 0 r0) (reg:SI 2 r2)) -1 (nil) (nil))\n")
            Path(wd, "diff.json").write_text(json.dumps([{
                "uid": 201, "rom": "adds r0, r1, #0", "draft": "adds r0, r2, #0",
                "tag": "~", "rom_addr": "08000000", "line": 1,
            }]))
            out = io.StringIO()
            with mock.patch.object(self.scope, "compile_draft", return_value=(
                    b"\0\0", 0x08000000, {"wd": wd, "obj_name": "Target"})), \
                    mock.patch.object(self.scope, "inventory", return_value=(
                        {}, {"Target": 0x08000000}, {"Target": 2})), \
                    mock.patch.object(self.scope, "load_rom", return_value=b"\0\0"), \
                    mock.patch.object(self.scope, "rom_name", return_value="Target"), \
                    mock.patch.object(self.scope, "report"), \
                    mock.patch.object(self.scope, "explain_alloc") as explain_alloc, \
                    contextlib.redirect_stdout(out):
                self.scope.cmd_why(str(source), "Target", 201, {})
            self.assertIn("regmove inserted 30 = 26 at insn 201 immediately before insn 27", out.getvalue())
            self.assertIn("Inspect the operand order before regmove", out.getvalue())
            self.assertIn("allocation advice omitted for this inserted copy", out.getvalue())
            explain_alloc.assert_not_called()
            self.assertNotIn("what-if: AGBCC_FORCE", out.getvalue())
            issues = self.scope.why_all_issues(out.getvalue())
            self.assertEqual(len(issues), 1)
            self.assertEqual(issues[0][0], ("regmove operand copy", 201, 27))
            self.assertIsNone(issues[0][2])
            self.assertIn("inspect operand order before regmove", issues[0][1])
