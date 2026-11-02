"""`preorder` explains PRE's stack-slot order with gcse's hash table and searches the table sizes and
pseudo renumberings that give the ROM's order."""
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope


def raw(r, o):
    return 13772 + r + o


class PreOrder(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_table_size_and_insn_counts(self):
        s = self.scope
        self.assertEqual(s.pre_table_size(1696), 849)
        self.assertEqual(s.pre_table_size(1705), 853)
        self.assertEqual(s.pre_table_size(10), 11)
        self.assertEqual(s.pre_table_insns(853), (1704, 1707))
        self.assertEqual(s.pre_table_insns(11), (0, 23))
        self.assertIsNone(s.pre_table_insns(852))
        for t in (13, 101, 853):
            lo, hi = s.pre_table_insns(t)
            self.assertTrue(all(s.pre_table_size(n) == t for n in range(lo, hi + 1)))
            self.assertNotEqual(s.pre_table_size(lo - 1), t)
            self.assertNotEqual(s.pre_table_size(hi + 1), t)

    def test_order_and_cut_search(self):
        s = self.scope
        # ROM order: r40+4 (index 1) before r22+8 (index 0). At T 11 the buckets are 0 and 8 (in order),
        # at T 13 they are 10 and 9 (out of order); moving r40 by +3 or -2 puts it in bucket 0 or 8.
        items = [(raw(40, 4), 1, 1), (raw(22, 8), 0, 0)]
        self.assertTrue(s.pre_order_ok(items, 11))
        self.assertFalse(s.pre_order_ok(items, 13))
        self.assertTrue(s.pre_order_ok(items, 13, [0, 3]))
        self.assertTrue(s.pre_order_ok(items, 13, [0, -2]))
        self.assertFalse(s.pre_order_ok(items, 13, [0, 1]))
        res = s.pre_order_search(items, [22, 40], {0}, 13, t_window=1, d_window=5)
        self.assertIn(11, res["t_only"])
        self.assertNotIn(13, res["t_only"])
        self.assertIn((1, 3, 13), res["cuts"])
        self.assertIn((1, -2, 13), res["cuts"])
        self.assertNotIn((1, 1, 13), res["cuts"])
        self.assertFalse(any(c == 0 for c, _, _ in res["cuts"]))  # the parameter's number is fixed

    def test_receiver_shift_moves_every_field_of_one_owner(self):
        # The category bucket must move before both turn fields. Bucket 1 is allowed
        # because its discovery index follows the first field. Bucket 4 is excluded
        # because its discovery index follows the first turn field as well.
        items = [(1, 0, 0), (6, 2, 1), (4, 1, 2), (5, 3, 2), (7, 4, 0)]
        self.assertEqual(self.scope.pre_receiver_shifts(items, 11), {1: [6, 7, 8]})
        room_after = [(1, 0, 0), (7, 1, 1), (3, 2, 2), (4, 3, 2), (9, 4, 0)]
        self.assertEqual(self.scope.pre_receiver_shifts(room_after, 11)[2], [4, 5])
        self.assertEqual(self.scope.pre_receiver_shifts([(1, 0, 0), (2, 1, 0)], 11), {})
        # Two disjoint inverted pairs cannot both be repaired by changing one owner.
        self.assertEqual(self.scope.pre_receiver_shifts(
            [(4, 0, 0), (1, 1, 1), (8, 2, 2), (5, 3, 3)], 11), {})

    def test_equal_buckets_fall_back_to_the_index(self):
        s = self.scope
        a, b = (raw(30, 0), 5, 0), (raw(30, 0) + 13, 2, 0)  # the same bucket at T 13
        self.assertFalse(s.pre_order_ok([a, b], 13))
        self.assertTrue(s.pre_order_ok([b, a], 13))

    def test_report_from_dumps(self):
        s = self.scope
        with tempfile.TemporaryDirectory() as wd:
            wd = Path(wd)
            # N = 24 insns into gcse -> T 13. Expression 0 is r22+8 (bucket 9), expression 1 r40+4 (bucket
            # 10): PRE makes 100 for the first and 101 for the second; the ROM has 101's slot first.
            insns = "".join(f"(insn {u} {u - 1} {u + 1} (set (reg:SI 50) (reg:SI 51)) -1 (nil))\n\n"
                            for u in range(10, 34))
            (wd / "u.i.addressof").write_text(";; Function F\n\n" + insns)
            (wd / "u.i.rtl").write_text(
                ';; Function F\n\n(note 1 0 0 ("f.c") 3)\n\n'
                "(insn 4 2 5 (set (reg/v:SI 22)\n        (reg:SI 0 r0)) -1 (nil))\n\n"
                '(note 5 4 6 "" NOTE_INSN_FUNCTION_BEG)\n\n(note 6 5 7 ("f.c") 7)\n\n'
                "(insn 8 6 9 (set (reg:SI 40)\n        (plus:SI (reg/v:SI 22) (const_int 64))) -1 (nil))\n\n")
            (wd / "u.i.gcse").write_text(
                "SET hash table (11 buckets, 1 entries)\nIndex 0 (hash value 3)\n  (set (reg:SI 50)\n"
                "    (reg:SI 51))\n\n"
                "Expression hash table (13 buckets, 2 entries)\n"
                f"Index 0 (hash value {raw(22, 8) % 13})\n  (plus:SI (reg/v:SI 22)\n    (const_int 8))\n"
                f"Index 1 (hash value {raw(40, 4) % 13})\n  (plus:SI (reg:SI 40)\n    (const_int 4))\n\n"
                "PRE: redundant insn 20 (expression 0) in bb 1, reaching reg is 100\n"
                "PRE: redundant insn 21 (expression 1) in bb 2, reaching reg is 101\n"
                "PRE GCSE of F, pass 1: 100 bytes needed, 2 substs, 2 insns created\n\n"
                "SET hash table (11 buckets, 1 entries)\nIndex 1 (hash value 4)\n  (set (reg:SI 52)\n"
                "    (const_int 0))\n")
            (wd / "names.txt").write_text("G F 22 1 1 1 1 1 actor\n")
            table = s.pre_table(str(wd), "F")
            self.assertEqual(table["T"], 13)
            self.assertEqual(table["reaching"], {100: 0, 101: 1})
            self.assertEqual(table["entries"][1][1], "(plus:SI (reg:SI 40) (const_int 4))")
            self.assertEqual(s.gcse_input_insns(str(wd), "F"), 24)
            self.assertEqual(s.parameter_pseudos(str(wd), "F"), {22})
            text = "\n".join(s.pre_order_report(str(wd), "F", [101, 100], "f.c", full=True))
            self.assertIn("N = 24 real insns into gcse, T = 13 buckets", text)
            self.assertIn("2/2 (plus:SI reg const) entries", text)
            self.assertIn("at T = 13: 1 adjacent pair(s) out of (bucket, index) order: 101 before 100", text)
            self.assertIn("T = 11 [N 0..23 (-24..-1)]", text)
            self.assertIn("from r40", text)
            self.assertIn("(line 7)", text)
            # in the draft's own order the table already explains it
            text = "\n".join(s.pre_order_report(str(wd), "F", [100, 101], "f.c"))
            self.assertIn("already rise in ROM order", text)

    def test_body_offset_loads(self):
        # The body loads a large offset into a register before the add (two insns); an inline copy's
        # add (insn/i) has the constant folded in, and small offsets are immediates: neither counts.
        s = self.scope
        with tempfile.TemporaryDirectory() as wd:
            wd = Path(wd)
            (wd / "u.i.addressof").write_text(
                ';; Function F\n\n(note 9 8 10 ("f.c") 5)\n\n'
                "(insn 10 9 11 (set (reg:SI 50)\n        (const_int 682)) -1 (nil)\n"
                "    (expr_list:REG_EQUAL (const_int 682)\n        (nil)))\n\n"
                "(insn 11 10 12 (set (reg:SI 51)\n        (plus:SI (reg/v:SI 22)\n"
                "            (reg:SI 50))) -1 (nil)\n    (nil))\n\n"
                "(insn/i 12 11 13 (set (reg:SI 52)\n        (const_int 690)) -1 (nil)\n    (nil))\n\n"
                "(insn/i 13 12 14 (set (reg:SI 53)\n        (plus:SI (reg/v:SI 22)\n"
                "            (reg:SI 52))) -1 (nil)\n    (nil))\n\n"
                "(insn 14 13 15 (set (reg:SI 54)\n        (const_int 100)) -1 (nil)\n    (nil))\n\n"
                "(insn 15 14 16 (set (reg:SI 55)\n        (plus:SI (reg/v:SI 22)\n"
                "            (reg:SI 54))) -1 (nil)\n    (nil))\n\n"
                '(note 16 15 17 ("f.c") 7)\n\n'
                "(insn 17 16 18 (set (reg:SI 56)\n        (const_int 1680)) -1 (nil)\n    (nil))\n\n"
                "(insn 18 17 19 (set (reg/v:SI 57)\n        (plus:SI (reg/v:SI 25)\n"
                "            (reg:SI 56))) -1 (nil)\n    (nil))\n\n"
                "(insn 19 18 20 (set (reg:SI 58)\n        (const_int 4096)) -1 (nil)\n    (nil))\n\n")
            self.assertEqual(s.body_offset_loads(str(wd), "F"), {("f.c", 5): 1, ("f.c", 7): 1})
            text = "\n".join(s.pre_n_levers(str(wd), "F", "f.c", 30, (20, 23)))
            self.assertIn("loads 2 large field offset(s)", text)
            self.assertIn("7 or more fewer", text)
            self.assertIn("Lines: 5, 7", text)
            self.assertEqual(s.pre_n_levers(str(wd), "F", "f.c", 30, (30, 33)), [])  # more insns: no lever
            self.assertEqual(s.pre_n_levers(str(wd), "F", "g.c", 30, (20, 23)), [])  # another file's lines


if __name__ == "__main__":
    unittest.main()
