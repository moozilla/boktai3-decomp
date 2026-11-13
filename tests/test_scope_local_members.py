"""Local allocation logs quantity heads, while lreg places every tied member."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest import mock

from tools.matchsearch.evaluator import _load_scope


class LocalMembers(unittest.TestCase):
    def setUp(self):
        self.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def fixture(self, wd, *, dead=True, extra_head=False, same_register=True):
        Path(wd, "names.txt").write_text(
            "L F 20 186 0 7777 4 22 7 0 2 address\n"
            + ("L F 20 30 1 4545 4 10 5 0 2 offset\n" if extra_head else "")
            + "G F 30 4545 5 22 0 2 offset\nG F 186 7000 7 18 0 2 address\n"
        )
        Path(wd, "u.i.lreg").write_text(
            ";; Register 30 in 2.\n;; Register 186 in %d.\n\n" % (2 if same_register else 3)
            + "(insn 431 430 434 (set (reg/v:SI 186)\n"
            " (plus:SI (reg/v:SI 22) (reg/v:SI 30))) 27 {addsi3} (nil)\n"
            + (" (expr_list:REG_DEAD (reg/v:SI 30) (nil)))\n" if dead else " (nil))\n")
        )
        Path(wd, "diff.json").write_text("[]")

    def test_tied_member_uses_local_quantity_record_and_knob_head(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd)
            records = self.scope.alloc_records(wd, "F")
            self.assertEqual(records[30], records[186])
            self.assertEqual(records[30][0], "L")
            self.assertEqual(self.scope.alloc_records(wd, "F", global_order=True)[30][0], "G")
            self.assertEqual(self.scope.kref(wd, "F", 30), "address")
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                self.scope.explain_alloc(wd, "F", 30, 3, 2, str, str)
            self.assertIn("shares the local quantity headed by 186", out.getvalue())
            self.assertNotIn("is a global-alloc pseudo", out.getvalue())

    def test_live_operand_is_not_a_tie(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd, dead=False)
            self.assertEqual(self.scope.alloc_records(wd, "F")[30][0], "G")

    def test_named_knobs_address_the_quantity_head(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd)
            with mock.patch.object(self.scope, "compile_draft"), \
                    mock.patch.object(self.scope, "workdir", return_value=wd):
                knobs = self.scope.resolve_knobs("unused.c", "F", {
                    "AGBCC_PRI": "offset:+1", "AGBCC_FORCE": "offset:r3",
                })
            self.assertEqual(knobs["AGBCC_PRI"], "186:+1")
            self.assertEqual(knobs["AGBCC_FORCE"], "186:3")

    def test_distinct_hard_registers_are_not_a_tie(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd, same_register=False)
            self.assertEqual(self.scope.alloc_records(wd, "F")[30][0], "G")

    def test_two_recorded_quantities_are_not_merged(self):
        with tempfile.TemporaryDirectory() as wd:
            self.fixture(wd, extra_head=True)
            records = self.scope.alloc_records(wd, "F")
            self.assertNotEqual(records[30], records[186])
            self.assertEqual(self.scope.kref(wd, "F", 30), "offset")
