"""`calls` and `pins` name a callee that reads more argument registers at its entry than the draft
declares it with (a pin on r1-r3 across the call is often that missing argument)."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def rows(*texts):
    return [(2 * i, None, t, None, None) for i, t in enumerate(texts)]


class MissingArgs(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_store_reads_both(self):
        self.assertEqual(self.scope.entry_arg_regs(rows("str r1, [r0, #0x30]", "movs r0, #1", "bx lr")), {0, 1})

    def test_written_first_is_not_an_argument(self):
        got = self.scope.entry_arg_regs(rows("push {r4, lr}", "adds r4, r0, #0", "movs r1, #2", "adds r2, #4",
                                             "bl #0x8000000", "ldr r3, [r4]"))
        self.assertEqual(got, {0, 2})

    def test_two_operand_alu_reads_its_destination(self):
        self.assertEqual(self.scope.entry_arg_regs(rows("lsls r3, r3, #2", "adds r1, #1", "movs r0, r2", "bx lr")),
                         {1, 2, 3})

    def test_declared_params(self):
        text = "void A(void *, void *);\nint B(void);\nint C();\nint D(int (*f)(int, int), int x) {"
        self.assertEqual(self.scope.declared_params(text, "A"), 2)
        self.assertEqual(self.scope.declared_params(text, "B"), 0)
        self.assertIsNone(self.scope.declared_params(text, "C"))
        self.assertEqual(self.scope.declared_params(text, "D"), 2)


if __name__ == "__main__":
    unittest.main()
