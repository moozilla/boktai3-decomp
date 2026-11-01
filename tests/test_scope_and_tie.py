"""scope's hint for an and/or whose tied operand differs (constant-first `&=`/`|=`, README fact 33)."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def row(tag, rom, draft, line="7"):
    return {"tag": tag, "rom": rom, "draft": draft, "line": line}


class AndTie(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_rom_ties_the_constant(self):
        rows = [row("~", "movs r1, #3", "movs r1, #3"), row("=", "lsrs r0, r0, #6", "lsrs r0, r0, #6"),
                row("~", "ands r1, r0", "ands r0, r1")]
        self.assertEqual(self.scope.and_tie_hints(rows), [(2, "7", "constant")])

    def test_negated_constant_through_rsbs(self):
        rows = [row("=", "movs r2, #0x21", "movs r2, #0x21"), row("=", "rsbs r2, r2, #0", "rsbs r2, r2, #0"),
                row("=", "ldr r0, [r4]", "ldr r0, [r4]"), row("~", "ands r2, r0", "ands r0, r2")]
        self.assertEqual(self.scope.and_tie_hints(rows), [(3, "7", "constant")])

    def test_rom_ties_the_value(self):
        rows = [row("=", "ldr r1, =0x3FF", "ldr r1, =0x3FF"), row("~", "ands r0, r1", "ands r1, r0")]
        self.assertEqual(self.scope.and_tie_hints(rows), [(1, "7", "value")])

    def test_same_text_other_roles(self):
        rows = [row("~", "ldrb r1, [r0]", "ldrb r0, [r0]"), row("~", "movs r0, #0xf", "movs r1, #0xf"),
                row("=", "ands r0, r1", "ands r0, r1")]
        self.assertEqual(self.scope.and_tie_hints(rows), [(2, "7", "constant")])

    def test_same_roles_give_nothing(self):
        rows = [row("=", "movs r0, #0xf", "movs r0, #0xf"), row("=", "ands r0, r1", "ands r0, r1")]
        self.assertEqual(self.scope.and_tie_hints(rows), [])

    def test_orrs_same_text_other_roles(self):
        rows = [row("~", "ldrb r1, [r2]", "ldrb r0, [r2]"), row("~", "movs r0, #4", "movs r1, #4"),
                row("=", "orrs r0, r1", "orrs r0, r1")]
        self.assertEqual(self.scope.and_tie_hints(rows), [(2, "7", "constant")])

    def test_mixed_ops_give_nothing(self):
        rows = [row("=", "movs r1, #3", "movs r1, #3"), row("~", "ands r1, r0", "orrs r0, r1")]
        self.assertEqual(self.scope.and_tie_hints(rows), [])

    def test_two_values_give_nothing(self):
        rows = [row("=", "ldr r1, [r4]", "ldr r1, [r4]"), row("~", "ands r0, r1", "ands r1, r0")]
        self.assertEqual(self.scope.and_tie_hints(rows), [])


if __name__ == "__main__":
    unittest.main()
