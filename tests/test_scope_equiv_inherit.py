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

    def missing_rows(self):
        return [{"tag": "=", "rom": "mov sl, r3", "draft": "mov sl, r3", "uid": 53},
                {"tag": "-", "rom": "mov r3, sl", "draft": None, "uid": None,
                 "rom_addr": "08001234"},
                {"tag": "=", "rom": "ands r0, r3", "draft": "ands r0, r3", "uid": 65}]

    def test_rom_only_inverse_copy_explains_reload_inheritance(self):
        hits = self.scope.missing_inherited_copies(self.missing_rows(), self.greg, self.lreg)
        self.assertEqual(len(hits), 1)
        self.assertEqual((hits[0]["pseudo"], hits[0]["constant"], hits[0]["load_uid"]), (29, 63, 647))

    def test_copy_removed_after_reload_is_a_different_mechanism(self):
        greg = self.greg[:2] + [(60, "insn", "(set (reg:SI 3 r3) (reg:SI 10 sl))")] + self.greg[2:]
        self.assertEqual(self.scope.missing_inherited_copies(self.missing_rows(), greg, self.lreg), [])

    def test_missing_copy_requires_adjacent_inverse_and_same_hard_register(self):
        rows = self.missing_rows()
        rows[0]["draft"] = "mov sb, r3"
        self.assertEqual(self.scope.missing_inherited_copies(rows, self.greg, self.lreg), [])
        rows = self.missing_rows()
        rows[1]["rom"] = "mov r3, r8"
        rows[0]["draft"] = "mov r8, r3"
        self.assertEqual(self.scope.missing_inherited_copies(rows, self.greg, self.lreg), [])

    def test_consumer_must_read_the_inherited_register(self):
        greg = self.greg[:2] + [(65, "insn", "(set (reg:SI 3 r3) (and:SI (reg:SI 0 r0) (reg:SI 1 r1)))")]
        self.assertEqual(self.scope.missing_inherited_copies(self.missing_rows(), greg, self.lreg), [])

    def test_rom_only_mechanism_enters_structured_summary(self):
        issues = self.scope.why_all_issues(
            "  reload inheritance for missing mov r3, sl: pseudo 29, consumer 65; already absent in greg/reload.\n")
        self.assertEqual(len(issues), 1)
        self.assertEqual((issues[0][0], issues[0][3]), (("reload inheritance", 29), 65))


if __name__ == "__main__":
    unittest.main()
