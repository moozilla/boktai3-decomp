"""`why` suggests writing a statement after a join again in each arm (jump2 merges the copies)."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class JoinLever(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def items(self):
        # EffectParticle_UpdateDissipating's shape: a (36) against p (26); an if/else whose arms both
        # reach label 916, then `entry = a->data + a->frame` (two mentions of a) at line 1314.
        return [
            ("insn", 100, (), {36: 0, 26: 1}),
            ("insn", 101, (), {36: 1, 26: 0}),
            ("jump_insn", 102, (900,), {36: 0, 26: 0}),
            ("insn", 103, (), {36: 1, 26: 0}),
            ("jump_insn", 104, (916,), {36: 0, 26: 0}),
            ("barrier", 105, (), {}),
            ("code_label", 900, (), {}),
            ("insn", 106, (), {36: 1, 26: 0}),
            ("code_label", 916, (), {}),
            ("insn", 919, (), {36: 1, 26: 0}),
            ("insn", 922, (), {36: 0, 26: 0}),
            ("insn", 924, (), {36: 1, 26: 0}),
            ("insn", 956, (), {36: 1, 26: 0}),
            ("insn", 990, (), {36: 0, 26: 1}),
        ]

    def test_statement_after_a_join_flips_the_order(self):
        lines = {919: 1314, 922: 1314, 924: 1314, 956: 1315, 916: 1313, 990: 1320}
        lo_rec = ("G", 96, 6400, 24, 150, 0, 4)
        hi_rec = ("G", 95, 6535, 40, 306, 0, 5)
        found = self.scope.join_lever(None, 36, lo_rec, 26, hi_rec, self.items(), lines)
        self.assertEqual(found[0][:5], (1313, [1314], 2, 26, 40))
        self.assertGreater(found[0][5], found[0][6])

    def test_no_join_no_lever(self):
        items = [it for it in self.items() if it[0] != "barrier"]
        items = [it for it in items if it[1] not in (102, 104, 900)]
        lines = {919: 1314, 922: 1314, 924: 1314, 956: 1315, 916: 1313}
        lo_rec = ("G", 96, 6400, 24, 150, 0, 4)
        hi_rec = ("G", 95, 6535, 40, 306, 0, 5)
        self.assertEqual(self.scope.join_lever(None, 36, lo_rec, 26, hi_rec, items, lines), [])


if __name__ == "__main__":
    unittest.main()
