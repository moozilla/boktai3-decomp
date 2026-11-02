"""`why` on a regmove tie (insn_list rows: uid, kind, pattern) names the u8 promoted-constant lever (README fact 54) when the op's other operand
is a small constant read again later."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope

ONE = "670 (set (reg/v:SI 300) (const_int 1))"
OR1 = "695 (set (reg:SI 315) (ior:SI (reg:SI 305) (reg/v:SI 300)))"
OR2 = "711 (set (reg:SI 320) (ior:SI (reg/v:SI 300) (reg:SI 319)))"
BIG = "671 (set (reg:SI 301) (const_int 3940))"
ADD = "696 (set (reg:SI 316) (and:SI (reg:SI 305) (reg:SI 301)))"


def insns(*rtls):
    return [(int(r.split()[0]), "insn", r.split(" ", 1)[1]) for r in rtls]


class ByteConstant(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_constant_read_again(self):
        self.assertEqual(self.scope.byte_constant_operand(insns(ONE, OR1, OR2), 695), (300, 1))

    def test_constant_dying_here_is_not_it(self):
        self.assertIsNone(self.scope.byte_constant_operand(insns(ONE, OR1), 695))

    def test_large_constant_is_not_it(self):
        self.assertIsNone(self.scope.byte_constant_operand(insns(BIG, ADD, ADD.replace("696 ", "699 ")), 696))

    def test_not_a_logical_op(self):
        self.assertIsNone(self.scope.byte_constant_operand(insns(ONE, BIG, OR2), 671))


if __name__ == "__main__":
    unittest.main()
