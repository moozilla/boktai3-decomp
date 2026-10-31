"""scope names the spilled pseudo behind a stack-slot reload and the pass that dropped its note, and
finds the dumps of a function renamed since by its sub_ label."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from tools.matchsearch.evaluator import _load_scope

SET = """(insn {uid} 1 2 (set (reg/v:SI 296)
        {src}) -1 (nil)
    {notes})

"""


class SlotReload(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def explain(self, rows, row):
        frame = "(plus:SI (reg:SI 7 r7) (const_int 28))"
        with tempfile.TemporaryDirectory() as wd:
            d = Path(wd)
            (d / "names.txt").write_text("S Func 296 64 4 5 -1\n")
            (d / "u.i.cse2").write_text(SET.format(uid=10, src="(reg:SI 529)",
                                                   notes=f"(expr_list:REG_EQUAL {frame}\n        (nil))"))
            for p in ("combine", "lreg"):
                (d / f"u.i.{p}").write_text(SET.format(uid=10, src=frame, notes="(nil)"))
            (d / "u.i.greg").write_text("(insn 1443 859 861 (set (reg:SI 3 r3)\n        (mem:SI (plus:SI (reg:SI 13 sp)\n"
                                        "                (const_int 64)) 0)) 1 {*movsi_insn} (nil)\n    (nil))\n\n")
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                self.scope.explain_slot_reload(wd, "Func", 1443, rows, row, str, lambda u: f"insn {u}")
        return out.getvalue()

    def test_names_the_pseudo_slot_and_the_pass_that_dropped_its_note(self):
        rows = [{"rom": "mov r3, sp", "draft": None, "uid": None},
                {"rom": None, "draft": "ldr r3, [sp, #0x40]", "uid": 1443}]
        text = self.explain(rows, rows[1])
        self.assertIn("pseudo 296 from its stack slot sp+64", text)
        self.assertIn("cse2: REG_EQUAL", text)
        self.assertIn("combine dropped the equivalence note", text)
        self.assertIn("recomputes a stack address", text)

    def test_no_stack_address_hint_without_one_in_the_rom(self):
        rows = [{"rom": None, "draft": "ldr r3, [sp, #0x40]", "uid": 1443}]
        self.assertNotIn("recomputes a stack address", self.explain(rows, rows[0]))

    def test_insn_lookup_accepts_the_sub_label_of_a_renamed_function(self):
        with tempfile.TemporaryDirectory() as work:
            Path(work, "Ratatosk_Update").mkdir()
            Path(work, "Ratatosk_Update", "u.i.rtl").write_text("")
            inventory = ({"sub_081F4CF8": "Ratatosk_Update"}, {"Ratatosk_Update": 0x081F4CF9}, {})
            with patch.object(self.scope, "WORK", work), \
                 patch.object(self.scope, "inventory", return_value=inventory), \
                 patch.object(self.scope, "last_func", return_value=None):
                self.assertEqual(self.scope.dumped_function("sub_081F4CF8"), "Ratatosk_Update")
                self.assertEqual(self.scope.dumped_function("081F4CF8"), "Ratatosk_Update")
                with self.assertRaises(SystemExit):
                    self.scope.dumped_function("sub_08000000")


if __name__ == "__main__":
    unittest.main()
