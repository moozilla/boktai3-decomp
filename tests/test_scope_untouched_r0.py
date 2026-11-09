import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools" / "scope"))
import scope  # noqa: E402


def rows(pairs):
    return [{"rom": a, "draft": b} for a, b in pairs]


class UntouchedR0Test(unittest.TestCase):
    def test_writes_r0(self):
        self.assertTrue(scope.writes_r0("adds r0, r3, #0"))
        self.assertTrue(scope.writes_r0("ldrh r0, [r1, #2]"))
        self.assertFalse(scope.writes_r0("strh r0, [r3, #0x1e]"))
        self.assertFalse(scope.writes_r0("cmp r0, #0"))
        self.assertFalse(scope.writes_r0("movs r0, r0"))
        self.assertFalse(scope.writes_r0("adds r3, r0, #0"))

    def test_hint_for_kept_r0(self):
        # BreakableBarrier_SetPosition: the ROM copies r0 out and never writes it; the draft returns.
        r = rows([("adds r3, r0, #0", "adds r3, r0, #0"), ("ldrh r3, [r1, #2]", "ldrh r0, [r1, #2]"),
                  ("blt func+0x10", "blt func+0x10"), ("bx r1", "adds r0, r3, #0")])
        self.assertTrue(scope.untouched_r0_hint(r))

    def test_no_hint_with_a_call_or_a_rom_write(self):
        r = rows([("adds r3, r0, #0", "adds r3, r0, #0"), ("bl Foo", "bl Foo"), ("bx r1", "adds r0, r3, #0")])
        self.assertFalse(scope.untouched_r0_hint(r))
        r = rows([("adds r3, r0, #0", "adds r3, r0, #0"), ("movs r0, #1", "movs r0, #1")])
        self.assertFalse(scope.untouched_r0_hint(r))


if __name__ == "__main__":
    unittest.main()
