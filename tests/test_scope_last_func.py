"""A file command with no FUNC, on a matched unit, means the function scope looked at last."""
from pathlib import Path
import os
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope

UNIT = """#include "global.h"
static inline int helper(int x)
{
    return x;
}
void Foo_First(void)
{
}
void Foo_Second(void)
{
}
void Foo_Third(void)
{
}
"""


class LastFunc(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.work = self.scope.WORK
        self.scope.WORK = self.tmp.name
        self.src = os.path.join(self.tmp.name, "unit.c")
        Path(self.src).write_text(UNIT)

    def tearDown(self):
        self.scope.WORK = self.work
        self.tmp.cleanup()

    def last(self, name):
        Path(self.tmp.name, "LAST").write_text(name)

    def test_without_a_record_the_last_definition(self):
        self.assertEqual(self.scope.nonmatching_func(self.src), "Foo_Third")

    def test_the_function_scoped_last(self):
        self.last("Foo_First")
        self.assertEqual(self.scope.nonmatching_func(self.src), "Foo_First")

    def test_a_record_from_another_file_is_ignored(self):
        self.last("Bar_Elsewhere")
        self.assertEqual(self.scope.nonmatching_func(self.src), "Foo_Third")

    def test_an_inline_is_never_chosen(self):
        self.last("helper")
        self.assertEqual(self.scope.nonmatching_func(self.src), "Foo_Third")

    def test_a_nonmatching_block_still_wins(self):
        Path(self.src).write_text(UNIT + "#ifdef NONMATCHING\nvoid Foo_Draft(void)\n{\n}\n#else\n#endif\n")
        self.last("Foo_First")
        self.assertEqual(self.scope.nonmatching_func(self.src), "Foo_Draft")


if __name__ == "__main__":
    unittest.main()
