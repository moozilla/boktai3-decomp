"""Pure commutation is an operand-order difference, not two allocation failures."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class CommutedOperands(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_game_state_base_add(self):
        self.assertTrue(self.scope.commuted_register_operands(
            "adds r1, r1, r0", "adds r1, r0, r1"))

    def test_aliases_and_multiply(self):
        self.assertTrue(self.scope.commuted_register_operands(
            "add ip, ip, r0", "add r12, r0, r12"))
        self.assertTrue(self.scope.commuted_register_operands(
            "muls r2, r0, r1", "muls r2, r1, r0"))

    def test_real_allocation_or_shape_differences(self):
        for rom, draft in [
            ("adds r1, r1, r0", "adds r0, r0, r1"),
            ("subs r1, r1, r0", "subs r1, r0, r1"),
            ("adds r1, r1, r0", "adds r1, r0, r2"),
            ("adds r1, r1, r0", "add r1, r0, r1"),
            ("add r1, r0", "add r0, r1"),
            ("adds r1, r1, #0", "adds r1, r0, #1"),
            ("adds r1, r0, r0", "adds r1, r0, r0"),
            (None, "adds r1, r0, r1"),
        ]:
            with self.subTest(rom=rom, draft=draft):
                self.assertFalse(self.scope.commuted_register_operands(rom, draft))

    def test_rtl_permutation_preserves_repeated_operands(self):
        self.assertTrue(self.scope.register_permutation([26, 26, 30], [26, 30, 26]))
        self.assertFalse(self.scope.register_permutation([26, 26, 30], [26, 30, 30]))
        self.assertFalse(self.scope.register_permutation([26, 30], [26, 31]))
        self.assertFalse(self.scope.register_permutation([26, 30], [26, 30]))


if __name__ == "__main__":
    unittest.main()
