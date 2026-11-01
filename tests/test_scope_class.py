"""`scope.py why` names a pseudo whose preferred register class has nothing allocatable in it."""
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope

LREG = """;; Function F

Register 23 used 4 times across 71 insns; set 1 time; user var; pref STACK_REG; pointer.

Register 27 used 4 times across 191 insns; set 1 time; user var; crosses 2 calls; pref LO_REGS; pointer.
"""


class ClassScope(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def setUp(self):
        self.dir = tempfile.TemporaryDirectory()
        Path(self.dir.name, "u.i.lreg").write_text(LREG)

    def tearDown(self):
        self.dir.cleanup()

    def test_pref_is_read_from_the_lreg_dump(self):
        self.assertEqual(self.scope.reg_pref(self.dir.name, 23), "STACK_REG")
        self.assertEqual(self.scope.reg_pref(self.dir.name, 27), "LO_REGS")
        self.assertIsNone(self.scope.reg_pref(self.dir.name, 99))

    def test_note_only_when_the_class_moves_it_across_low_and_high(self):
        self.assertIn("STACK_REG", self.scope.class_note(self.dir.name, 23, 12, 6))
        self.assertEqual(self.scope.class_note(self.dir.name, 23, 5, 6), "")
        self.assertEqual(self.scope.class_note(self.dir.name, 27, 12, 6), "")

    def test_misplaced_skips_registers_the_rom_uses(self):
        s = self.scope
        saved = s.hard_of, s.alloc_records, s.pseudo_names
        try:
            s.hard_of = lambda wd: {23: 12, 27: 9}
            s.alloc_records = lambda wd, func: {23: ("G", 0, 1126, 4, 71, 0, 12),
                                                27: ("G", 1, 418, 4, 191, 2, 9)}
            s.pseudo_names = lambda wd, func: {23: "p"}
            lines = s.class_misplaced(self.dir.name, "F", "adds r6, r0, #0 mov sb, r0")
            self.assertEqual(len(lines), 1)
            self.assertTrue(lines[0].startswith("class: 23 (p) prefers STACK_REG and sits in ip"))
            self.assertEqual(s.class_misplaced(self.dir.name, "F", "mov ip, r0"), [])
        finally:
            s.hard_of, s.alloc_records, s.pseudo_names = saved


if __name__ == "__main__":
    unittest.main()
