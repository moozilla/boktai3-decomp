"""`why all` names the ROM's -1-through-a-spill-register copy (a QImode pseudo) and the C that gives it."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def row(tag, rom, draft, line):
    return {"tag": tag, "rom": rom, "draft": draft, "line": line}


class MinusOneCopy(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_rom_copy_missing_from_draft(self):
        rows = [row("=", "ands r0, r3", "ands r0, r3", "87"),
                row("!", "movs r2, #1", "movs r1, #0xff", "89"),
                row("-", "rsbs r2, r2, #0", None, None),
                row("-", "adds r1, r2, #0", None, None),
                row("=", "cmp r0, #0", "cmp r0, #0", "90")]
        got = self.scope.minus_one_copy(rows)
        self.assertEqual(len(got), 1)
        self.assertIn("p->field = c ? 1 : -1;", got[0])
        self.assertIn("line 89", got[0])

    def test_draft_with_the_copy_is_quiet(self):
        rows = [row("=", "movs r2, #1", "movs r3, #1", "89"),
                row("=", "rsbs r2, r2, #0", "rsbs r3, r3, #0", "89"),
                row("=", "adds r1, r2, #0", "adds r1, r3, #0", "89")]
        self.assertEqual(self.scope.minus_one_copy(rows), [])

    def test_plain_negation_is_not_a_copy(self):
        rows = [row("-", "movs r2, #1", None, "89"), row("-", "rsbs r2, r2, #0", None, None),
                row("-", "adds r2, r2, #0", None, None)]
        self.assertEqual(self.scope.minus_one_copy(rows), [])


if __name__ == "__main__":
    unittest.main()
