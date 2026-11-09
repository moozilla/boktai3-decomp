"""`scope.py` names the ROM's `orrs rX, rZero` that a two-statement u16 field update leaves."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def row(rom, draft, line=None):
    tag = "=" if rom == draft else ("-" if draft is None else "+" if rom is None else "!")
    return {"rom": rom, "draft": draft, "tag": tag, "line": line}


class ZeroOrr(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def head(self):
        return [row("movs r4, #0", "movs r4, #0", 40),
                row("strh r4, [r0]", "strh r4, [r0]", 40),
                row("bl CpuSet", "bl CpuSet", 40),
                row("cmp r0, #0", "cmp r0, #0", 41),
                row("beq func+0x48", "beq func+0x48", 41),
                row("ldrh r0, [r7, #0x22]", "ldrh r0, [r7, #0x22]", 46),
                row("movs r1, #8", "movs r1, #8", 46),
                row("orrs r0, r1", "orrs r0, r1", 46)]

    def test_lost_zero_orr_is_named(self):
        rows = self.head() + [row("orrs r0, r4", None), row("ands r0, r1", "ands r0, r1", 47)]
        self.assertEqual(self.scope.zero_orr_hints(rows), [(8, 47, 4)])

    def test_same_block_zero_gives_nothing(self):
        rows = [r for r in self.head() if not r["rom"].startswith("beq")]
        rows += [row("orrs r0, r4", None), row("ands r0, r1", "ands r0, r1", 47)]
        self.assertEqual(self.scope.zero_orr_hints(rows), [])

    def test_kept_orr_gives_nothing(self):
        rows = self.head() + [row("orrs r0, r4", "orrs r0, r4", 46)]
        self.assertEqual(self.scope.zero_orr_hints(rows), [])


if __name__ == "__main__":
    unittest.main()
