from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools" / "scope"))

import scope  # noqa: E402


FLOW = "Register 70 used 6 times across 48 insns.\n"
SET = "(insn 1{n} 0 0 (set (reg/v:SI 70) (symbol_ref:SI (\"gValues\"))) -1 (nil)\n    (expr_list:REG_EQUIV (symbol_ref:SI (\"gValues\")) (nil)))\n"


class EquivLifeTest(unittest.TestCase):
    def note(self, sets, live):
        with tempfile.TemporaryDirectory() as wd:
            Path(wd, "u.i.flow").write_text(FLOW)
            Path(wd, "u.i.lreg").write_text("\n\n".join(SET.format(n=i) for i in range(sets)))
            return scope.life_note(wd, 70, live)

    def test_two_equiv_sets_quadruple_the_length(self):
        note = self.note(2, 192)
        self.assertIn("2 sets with a REG_EQUIV note (x4)", note)
        self.assertNotIn("regmove", note)

    def test_one_equiv_set_doubles(self):
        self.assertIn("always holds a constant", self.note(1, 96))

    def test_other_changes_stay_regmove(self):
        self.assertIn("regmove", self.note(2, 100))


if __name__ == "__main__":
    unittest.main()
