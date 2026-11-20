"""Register reads depend on the operand form, including immediate operands."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class AsmReads(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_countdown_reads_its_initial_value(self):
        rows = [
            {"draft": "adds r6, r1, #0"},
            {"draft": "ldrh r0, [r4]"},
            {"draft": "subs r6, #1"},
            {"draft": "cmp r6, #0"},
        ]
        self.assertEqual(self.scope.readers_after(rows, 0, "draft", 6), [2])

    def test_two_register_arithmetic_reads_destination(self):
        self.assertEqual(self.scope.asm_reads("ands r3, r4"), [4, 3])
        self.assertEqual(self.scope.asm_reads("add ip, r1"), [1, 12])

    def test_three_operand_arithmetic_uses_explicit_sources(self):
        self.assertEqual(self.scope.asm_reads("adds r6, r1, #0"), [1])
        self.assertEqual(self.scope.asm_reads("adds r6, r1, r3"), [1, 3])

    def test_unary_instruction_does_not_read_previous_destination(self):
        self.assertEqual(self.scope.asm_reads("mvns r6, r1"), [1])
        self.assertEqual(self.scope.asm_reads("movs r6, #0"), [])


if __name__ == "__main__":
    unittest.main()
