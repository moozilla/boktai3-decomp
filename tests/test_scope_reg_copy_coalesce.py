"""`why` finds the copy regmove's optimize_reg_copy_1 coalesced (it does not log)."""
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope

DUMP = """(insn 528 525 531 (set (reg/v:SI 38)
        (mem:SI (reg/v:SI 24) 2)) 1 {*movsi_insn} (nil)
    (nil))

(insn 531 528 532 (set (reg/v:SI 150)
        (reg/v:SI 38)) 1 {*movsi_insn} (insn_list 528 (nil))
    (nil))

(note 532 531 533 ("u.c") 131)

(insn 533 532 535 (set (reg:SI 210)
        (plus:SI (reg/v:SI 38)
            (const_int 1))) 27 {addsi3} (nil)
    (expr_list:REG_DEAD (reg/v:SI 38)
        (nil)))

"""


class RegCopyCoalesce(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def found(self, dump, uid=533):
        with tempfile.TemporaryDirectory() as wd:
            Path(wd, "u.i.combine").write_text(dump)
            return self.scope.reg_copy_coalesce(wd, uid, [38], [150])

    def test_copy_before_death(self):
        self.assertEqual(self.found(DUMP), (531, 150, 38))

    def test_label_between_blocks_it(self):
        dump = DUMP.replace('(note 532 531 533 ("u.c") 131)', '(code_label 532 531 533 7 "")')
        self.assertIsNone(self.found(dump))

    def test_in_place_set_between_blocks_it(self):
        dump = DUMP.replace('(note 532 531 533 ("u.c") 131)',
                            "(insn 532 531 533 (set (reg/v:SI 38)\n        (plus:SI (reg/v:SI 38)\n"
                            "            (const_int 1))) 27 {addsi3} (nil)\n    (nil))")
        self.assertIsNone(self.found(dump))

    def test_needs_death_at_use(self):
        self.assertIsNone(self.found(DUMP.replace("REG_DEAD", "REG_EQUAL")))


if __name__ == "__main__":
    unittest.main()
