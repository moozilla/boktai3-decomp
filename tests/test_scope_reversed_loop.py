"""`scope.py` names a loop that loop.c reversed on one side only (README fact 40)."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def row(rom, draft, line=107, born="rtl", addr="081BE660"):
    tag = "=" if rom == draft else ("-" if draft is None else "+" if rom is None else "!")
    return {"rom": rom, "draft": draft, "tag": tag, "line": line, "born": born,
            "rom_addr": addr if rom is not None else None}


class ReversedLoop(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_draft_reversal_from_loop_pass(self):
        rows = [row("ldr r3, [sp, #4]", None),
                row("adds r3, #1", None),
                row("str r3, [sp, #4]", None),
                row(None, "ldr r0, [sp]", born="greg"),
                row(None, "subs r0, #1", born="loop"),
                row(None, "str r0, [sp]", born="greg"),
                row("cmp r3, #3", "cmp r0, #0", born="loop"),
                row("bgt func+0x162", None),
                row(None, "blt func+0x166", born="loop")]
        self.assertEqual(self.scope.reversed_loop_hints(rows), [(6, 107, "draft")])

    def test_draft_reversal_without_pass_names(self):
        # the build's bytes differ from the -g compile's: no born or line, a high-register counter
        rows = [row("ldr r1, [sp]", None, line=None, born=None, addr="081BE6E2"),
                row("adds r1, #1", None, line=None, born=None, addr="081BE6E4"),
                row(None, "movs r1, #1", line=None, born=None),
                row(None, "rsbs r1, r1, #0", line=None, born=None),
                row(None, "add sl, r1", line=None, born=None),
                row(None, "mov r2, sl", line=None, born=None),
                row(None, "cmp r2, #0", line=None, born=None),
                row(None, "bge func+0x186", line=None, born=None),
                row("str r1, [sp]", "str r7, [sp]", line=None, born=None, addr="081BE6E6"),
                row("cmp r1, #3", None, line=None, born=None, addr="081BE6E8")]
        self.assertEqual(self.scope.reversed_loop_hints(rows), [(6, "at ROM 081BE6E8", "draft")])

    def test_rom_reversal(self):
        rows = [row("subs r2, #1", "adds r2, #1"),
                row("str r2, [sp]", "str r2, [sp]"),
                row("cmp r2, #0", "cmp r2, #3"),
                row("bge func+0x40", "ble func+0x40")]
        self.assertEqual(self.scope.reversed_loop_hints(rows), [(2, 107, "rom")])

    def test_matching_counters_give_nothing(self):
        rows = [row("adds r3, #1", "adds r3, #1"),
                row("cmp r3, #3", "cmp r3, #3"),
                row("ble func+0x56", "ble func+0x56"),
                row("cmp r0, #0", "cmp r0, #0", born="rtl")]
        self.assertEqual(self.scope.reversed_loop_hints(rows), [])

    def test_unrelated_zero_compare_gives_nothing(self):
        # a draft test against 0 that no decrement feeds and loop.c did not make
        rows = [row("ldrsb r0, [r1, r0]", "ldrsb r0, [r1, r0]"),
                row("cmp r0, #1", "cmp r0, #0", born="rtl"),
                row("bge func+0x122", "bge func+0x126")]
        self.assertEqual(self.scope.reversed_loop_hints(rows), [])


if __name__ == "__main__":
    unittest.main()
