"""`why`'s ref lever names the refs that sit in tails jump2 cross-jumps away."""
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope


def insn(uid, pat):
    return f"(insn {uid} {uid - 1} {uid + 1} {pat})\n"


def dump(*blocks):
    return "\n".join(blocks) + "\n"


class CrossJumpRefs(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def write(self, wd, name, text):
        (Path(wd) / ("u.i." + name)).write_text(text)

    def test_refs_in_a_cross_jumped_tail(self):
        with tempfile.TemporaryDirectory() as wd:
            line = '(note 1 0 0 ("a.c") 7)\n'
            a = insn(10, "(set (reg:SI 30) (plus:SI (reg/v:SI 22) (const_int 4)))")
            b = insn(11, "(set (reg:SI 31) (plus:SI (reg/v:SI 22) (const_int 8)))")
            self.write(wd, "regmove", dump(line, a, b))
            self.write(wd, "greg", dump(line, a, b))
            self.write(wd, "jump2", dump(line, a))  # insn 11 was a duplicate tail
            text = self.scope.mention_lines(wd, 22)
            self.assertIn("1 of them in tails jump2 cross-jumps away (line 7)", text)

    def test_no_note_without_deleted_tails(self):
        with tempfile.TemporaryDirectory() as wd:
            a = insn(10, "(set (reg:SI 30) (plus:SI (reg/v:SI 22) (const_int 4)))")
            for p in ("regmove", "greg", "jump2"):
                self.write(wd, p, dump(a))
            self.assertNotIn("cross-jumps", self.scope.mention_lines(wd, 22))


if __name__ == "__main__":
    unittest.main()
