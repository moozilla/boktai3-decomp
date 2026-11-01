"""scope names jump2's cross-jumps next to a difference, and why a tail the ROM shares was not (fact 39)."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def row(tag, draft, uid=None, line=""):
    return {"tag": tag, "rom": None if tag == "+" else draft, "draft": None if tag == "-" else draft,
            "uid": uid, "line": line}


class CrossJump(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_trace_lines_of_this_function(self):
        text = ("CROSS F jump 7 target 9: share 3 with 5\n"
                "CROSS G jump 8 target 9: share 4 with 6\n"
                "CROSS jump 1 target 2: share 3 with 4\n")
        self.assertEqual(self.scope.cross_merges(text, "F"), [(7, 9, 3, 5), (1, 2, 3, 4)])

    def test_merge_next_to_rom_code_is_named(self):
        rows = [row("=", "movs r0, #0", 2, "72"), row("-", "str r0, [r4, #0x44]"),
                row("-", "movs r0, #1"), row("→", "b func+0x14C", 7, "75")]
        order = [2, 3, 4, 5, 6, 7, 20, 30]
        kinds = {7: "jump_insn", 30: "jump_insn", 40: "code_label"}
        out = self.scope.cross_jump_hints(rows, [(7, 30, 3, 20)], order, kinds, {7: 75, 30: 81})
        self.assertEqual(out, [(75, 4, 81, "jump", True)])

    def test_return_value_merge_is_left_out(self):
        rows = [row("-", "movs r0, #0"), row("→", "b func+0x19E", 7, "63")]
        kinds = {7: "jump_insn", 40: "code_label"}
        self.assertEqual(self.scope.cross_jump_hints(rows, [(7, 40, 6, 50)], [5, 6, 7, 50], kinds, {}), [])

    def test_unmerged_tail_names_the_nop(self):
        rows = [row("+", "strb r0, [r1]", 5, "79"), row("+", "bl Memory_Clear", 6, "80"),
                row("→", "b func+0x1A4", 7, "81")]
        seq = [(5, "insn", "(set (mem:QI"), (6, "call_insn", "(call (mem:SI"),
               (7, "jump_insn", "(set (pc)\n        (label_ref 12)) 47 {jump}"), (8, "barrier", ""),
               (10, "call_insn", "(call (mem:SI"), (11, "insn", "(use (const_int 0)) -1 (nil)"),
               (12, "code_label", "28 (\"entered\"))")]
        kinds = {u: k for u, k, _ in seq}
        out = self.scope.cross_jump_hints(rows, [], [5, 6, 7, 10], kinds, {7: 81}, seq)
        self.assertEqual(out, [(81, None, None, None, "nop")])


if __name__ == "__main__":
    unittest.main()
