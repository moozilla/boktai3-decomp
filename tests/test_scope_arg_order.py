"""`scope.py` names a call argument the ROM precomputes (set first, before the others)."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def row(rom, draft, line=93):
    tag = "=" if rom == draft else ("-" if draft is None else "+" if rom is None else "!")
    return {"rom": rom, "draft": draft, "tag": tag, "line": line}


class ArgOrder(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_precomputed_argument_is_named(self):
        rows = [row("beq func+0x86", "beq func+0x86"),
                row("adds r3, r4, #0", None),
                row("adds r0, r7, #0", "adds r0, r7, #0"),
                row("movs r1, #3", "movs r1, #3"),
                row("movs r2, #2", "movs r2, #2"),
                row(None, "adds r3, r4, #0"),
                row("bl #0x807c72c", "bl #0x807c72c")]
        self.assertEqual(self.scope.arg_order_hints(rows), [(6, 93, 3, "precomputed")])

    def test_same_order_gives_nothing(self):
        rows = [row("adds r0, r7, #0", "adds r0, r7, #0"),
                row("movs r1, #3", "movs r1, #3"),
                row("bl #0x807c72c", "bl #0x807c72c")]
        self.assertEqual(self.scope.arg_order_hints(rows), [])

    def test_different_registers_give_nothing(self):
        rows = [row("movs r1, #3", "movs r2, #3"),
                row("adds r0, r7, #0", "adds r0, r7, #0"),
                row("bl #0x807c72c", "bl #0x807c72c")]
        self.assertEqual(self.scope.arg_order_hints(rows), [])

    def test_constant_loaded_early_is_named(self):
        # BoardPiece_Initialize: the ROM copies r0 then loads the literal; the draft's constant
        # pseudo took r1 at its set, ahead of the copy.
        rows = [row("bl #0x82151e4", "bl #0x82151e4"),
                row(None, "ldr r1, =0x2D3"),
                row("adds r0, r4, #0", "adds r0, r4, #0"),
                row("ldr r1, =0x2D3", None),
                row("bl #0x8215284", "bl #0x8215284")]
        self.assertEqual(self.scope.arg_order_hints(rows), [(4, 93, 0, "constant")])


if __name__ == "__main__":
    unittest.main()
