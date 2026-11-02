"""`scope.py uses rN` accepts a hard register as well as a variable name."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class HardRegArg(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_numbered(self):
        self.assertEqual(self.scope.hard_reg_arg("r0"), 0)
        self.assertEqual(self.scope.hard_reg_arg("r4"), 4)
        self.assertEqual(self.scope.hard_reg_arg("r12"), 12)

    def test_aliases(self):
        self.assertEqual(self.scope.hard_reg_arg("sb"), 9)
        self.assertEqual(self.scope.hard_reg_arg("ip"), 12)

    def test_not_allocatable_or_variable(self):
        for arg in ("r13", "sp", "lr", "pc", "rate", "r", "r4x"):
            self.assertIsNone(self.scope.hard_reg_arg(arg), arg)


if __name__ == "__main__":
    unittest.main()
