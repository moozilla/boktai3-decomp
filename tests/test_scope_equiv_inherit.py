"""scope's explanation of find_equiv_reg inheriting a REG_EQUIV constant (README fact 43)."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class EquivInherit(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    # ContactSequenceActor_UpdateDying's draft: xzMask (29) = 63 lives in sl; the and reuses r3.
    greg = [(647, "insn", "(set (reg:SI 3 r3) (const_int 63))"),
            (53, "insn", "(set (reg/v:SI 10 sl) (reg:SI 3 r3))"),
            (65, "insn", "(set (reg:SI 0 r0) (and:SI (reg:SI 0 r0) (reg:SI 3 r3)))")]
    lreg = [(53, "insn", "(set (reg/v:SI 29) (const_int 63))"),
            (65, "insn", "(set (reg:SI 41) (and:SI (reg:SI 41) (reg/v:SI 29)))")]

    def test_constant_reload_register_reused(self):
        self.assertEqual(self.scope.equiv_constant_inherit(self.greg, self.lreg, 65, 3), (29, 63, 647, 53))

    def test_register_overwritten_in_between(self):
        greg = self.greg[:2] + [(60, "insn", "(set (reg:SI 3 r3) (reg:SI 1 r1))")] + self.greg[2:]
        self.assertIsNone(self.scope.equiv_constant_inherit(greg, self.lreg, 65, 3))

    def test_call_in_between(self):
        call = (60, "call_insn", '(call (mem:SI (symbol_ref:SI ("f"))) (const_int 0))')
        greg = self.greg[:2] + [call] + self.greg[2:]
        self.assertIsNone(self.scope.equiv_constant_inherit(greg, self.lreg, 65, 3))

    def test_variable_set_twice_has_no_equivalence(self):
        lreg = [(52, "insn", "(set (reg/v:SI 29) (const_int 63))")] + self.lreg
        self.assertIsNone(self.scope.equiv_constant_inherit(self.greg, lreg, 65, 3))

    def test_other_constant(self):
        lreg = [(53, "insn", "(set (reg/v:SI 29) (const_int 127))"), self.lreg[1]]
        self.assertIsNone(self.scope.equiv_constant_inherit(self.greg, lreg, 65, 3))


if __name__ == "__main__":
    unittest.main()
