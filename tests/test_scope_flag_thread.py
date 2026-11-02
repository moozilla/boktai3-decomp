"""`scope.py` names a materialized 0/1 flag the ROM keeps and the draft's jumps thread past."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def row(rom, draft, line=None):
    tag = "=" if rom == draft else ("-" if draft is None else "+" if rom is None else "!")
    return {"rom": rom, "draft": draft, "tag": tag, "line": line}


class FlagThread(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_lost_flag_is_named(self):
        rows = [row("cmp r2, #0x17", "cmp r2, #0x17", 30),
                row("bne func+0x9A", "bne func+0xA4", 30),
                row("movs r0, #1", None),
                row("b func+0x9C", None),
                row("movs r0, #0", None),
                row("cmp r0, #0", None),
                row("beq func+0xAC", None),
                row("ldr r0, =0x2000580", "ldr r0, =0x2000580", 54)]
        self.assertEqual(self.scope.flag_thread_hints(rows), [(2, 54)])

    def test_pool_between_is_skipped(self):
        rows = [row("movs r0, #1", None, 61),
                row("b func+0x116", None),
                row("movs r0, r0", None),
                row(".word 0x000002BE", None),
                row("movs r0, #0", None),
                row("cmp r0, #0", "cmp r0, #0", 61)]
        self.assertEqual(self.scope.flag_thread_hints(rows), [(0, 61)])

    def test_kept_flag_gives_nothing(self):
        rows = [row(t, t, 5) for t in ("movs r0, #1", "b func+0x9C", "movs r0, #0", "cmp r0, #0")]
        self.assertEqual(self.scope.flag_thread_hints(rows), [])


if __name__ == "__main__":
    unittest.main()
