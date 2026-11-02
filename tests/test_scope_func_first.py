"""`scope.py FILE.c FUNC insn UID` (and vs/uses/life/put) takes the function first too."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class FuncFirst(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_function_moves_to_the_end(self):
        f = self.scope.func_first_args
        self.assertEqual(f(["a.c", "Foo_Bar", "insn", "198"]), ["a.c", "insn", "198", "Foo_Bar"])
        self.assertEqual(f(["a.c", "Foo_Bar", "life", "x", "y"]), ["a.c", "life", "x", "y", "Foo_Bar"])
        self.assertEqual(f(["a.c", "Foo", "vs", "b.c"]), ["a.c", "vs", "b.c", "Foo"])

    def test_knobs_are_kept_in_place(self):
        f = self.scope.func_first_args
        self.assertEqual(f(["a.c", "AGBCC_PRI=x:+1", "Foo", "insn", "5"]),
                         ["a.c", "AGBCC_PRI=x:+1", "insn", "5", "Foo"])

    def test_other_forms_are_unchanged(self):
        f = self.scope.func_first_args
        for args in (["a.c", "insn", "198", "Foo"], ["a.c", "Foo", "why", "all"],
                     ["a.c", "life", "insn", "x"], ["rom", "Foo"], ["a.c", "Foo"]):
            self.assertEqual(f(args), args)


if __name__ == "__main__":
    unittest.main()
