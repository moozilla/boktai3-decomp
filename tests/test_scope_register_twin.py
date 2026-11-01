"""`why` pairs a '+' row with the '-' row it stands for when only high registers kept them apart."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def row(tag, rom=None, draft=None, addr=None):
    return {"tag": tag, "rom": rom, "draft": draft, "rom_addr": addr}


class RegisterTwin(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_swap_pairs_by_position_and_shape(self):
        rows = [
            row(" ", "movs r0, #1", "movs r0, #1", "0x10"),
            row("-", "mov ip, r2", None, "0x12"),
            row("-", "str r0, [sp]", None, "0x14"),
            row("+", None, "mov sb, r2"),
            row("+", None, "str r0, [sp, #4]"),
            row(" ", "bx lr", "bx lr", "0x16"),
        ]
        twin = self.scope.register_twin(rows, rows[3])
        self.assertIs(twin, rows[1])
        self.assertIsNone(self.scope.register_twin(rows, rows[4]))

    def test_twin_after_the_plus_run(self):
        rows = [
            row("+", None, "add r1, sb"),
            row("-", "add r1, ip", None, "0x20"),
        ]
        self.assertIs(self.scope.register_twin(rows, rows[0]), rows[1])

    def test_low_and_high_register_copies_pair(self):
        # a copy into a low register is `adds rD, rS, #0`, into a high one `mov rD, rS`
        rows = [
            row("-", "mov r8, r6", None, "0x30"),
            row("+", None, "adds r7, r6, #0"),
        ]
        self.assertIs(self.scope.register_twin(rows, rows[1]), rows[0])

    def test_only_plus_rows_with_a_draft(self):
        rows = [row("-", "mov ip, r2", None, "0x12"), row("+", None, "mov sb, r2")]
        self.assertIsNone(self.scope.register_twin(rows, rows[0]))
        self.assertIsNone(self.scope.register_twin(rows, None))


if __name__ == "__main__":
    unittest.main()
