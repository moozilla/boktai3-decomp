"""`why` on a high-register reload move: pair it with the ROM-only line and name the pseudo behind it."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class ReloadMove(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_shape_blanks_registers(self):
        s = self.scope
        self.assertEqual(s.insn_shape("mov sl, r1"), s.insn_shape("mov sb, r1"))
        self.assertNotEqual(s.insn_shape("mov sl, r1"), s.insn_shape("ldr r1, =0x2000710"))

    def test_inserted_line_pairs_with_neighbouring_rom_line(self):
        rows = [{"tag": "=", "rom": "strb r0, [r1]", "draft": "strb r0, [r1]", "uid": 52},
                {"tag": "-", "rom": "mov sb, r1", "draft": None, "uid": None, "rom_addr": "0818D24E"},
                {"tag": "+", "rom": None, "draft": "mov sl, r1", "uid": 519, "rom_addr": None},
                {"tag": "+", "rom": None, "draft": "ldr r1, =0x2000710", "uid": 613, "rom_addr": None}]
        got = self.scope.pair_rom_only(rows, rows[2])
        self.assertEqual((got["rom"], got["tag"], got["rom_addr"]), ("mov sb, r1", "~", "0818D24E"))
        self.assertIsNone(self.scope.pair_rom_only(rows, rows[3]))
        self.assertIsNone(self.scope.pair_rom_only(rows, rows[0]))

    def test_reload_move_names_the_pseudo_in_that_register(self):
        order = [37, 519, 52, 60]
        pre = {37: "(set (reg:SI 36) (plus:SI (reg:SI 30) (reg:SI 31)))",
               52: "(set (mem:QI (reg:SI 36) 0) (reg:QI 40))", 60: "(set (reg:SI 41) (reg:SI 42))"}
        hmap = {36: 10, 40: 0, 30: 7}
        self.assertEqual(self.scope.reload_move_pseudo(None, 519, 10, order, pre, hmap), 36)
        self.assertIsNone(self.scope.reload_move_pseudo(None, 519, 9, order, pre, hmap))

    def test_holders_allocated_later_do_not_explain_a_conflict(self):
        recs = {36: ("G", 70, 1126, 4, 71, 1, 10), 169: ("G", 72, 1111, 3, 27, 1, 9)}
        f = self.scope.later_holders_only
        self.assertTrue(f({169: 310}, [], recs[36], recs))
        self.assertFalse(f({169: 310}, [12], recs[36], recs))
        self.assertFalse(f({9: 310}, [], recs[36], recs))


if __name__ == "__main__":
    unittest.main()
