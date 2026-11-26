"""Field identity and instruction order in a short Thumb trace."""
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from flag_order import Address, BASE, sample


class FlagOrder(unittest.TestCase):
    def rows(self, body, joins=()):
        lines = [f"{BASE + i * 2:x}: {insn}" for i, insn in enumerate(body)]
        return list(sample(lines, b"", {BASE: "tick"}, set(joins),
                           {"tick": ("src/a.c", {"r0": Address("Actor", "r0", 0)})},
                           {("Actor", 4, 2): "Actor.flags"}, {}).values())

    def test_load_order_and_same_address_writeback(self):
        for first, second, early in (("movs r2, #4", "ldrh r1, [r0, #4]", True),
                                     ("ldrh r1, [r0, #4]", "movs r2, #4", False)):
            op = [first, second, "orrs r1, r2"]
            row, = self.rows(op + ["strh r1, [r0, #4]"])
            self.assertEqual(row[2], "set")
            self.assertEqual(row[3].mask.ready < row[3].word.loaded, early)
            for store in ("strh r1, [r0, #6]", "str r1, [r0, #4]"):
                self.assertEqual(self.rows(op + [store]), [])

    def test_conditional_consumer_and_basic_block_boundary(self):
        test = ["movs r2, #8", "ldrh r1, [r0, #4]", "ands r1, r2", "cmp r1, #0"]
        self.assertEqual(self.rows(test), [])
        self.assertEqual(self.rows(test + ["beq.n 8000024"])[0][2], "test")
        self.assertEqual(self.rows(test + ["beq.n 8000024"], [BASE + 2]), [])
