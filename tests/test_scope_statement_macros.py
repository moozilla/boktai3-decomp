"""`scope.py FILE.c pins` names the do/while(0) statement macros a function uses."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope

SRC = """#define SET_MASK(out_)                                                   \\
    do {                                                                   \\
        (out_) = 0x200;                                                    \\
    } while (0)
#define PLAIN(x) ((x) + 1)
void Other(void) { u32 m; SET_MASK(m); }
void Target(void)
{
    u32 m;
    SET_MASK(m);
    m = PLAIN(m);
}
"""


class StatementMacros(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_macro_used_in_the_function_is_named(self):
        start = SRC.index("void Target")
        reach = {"Target": (start, len(SRC))}
        self.assertEqual(self.scope.statement_macros(SRC, reach, None), [("SET_MASK", 10)])

    def test_uses_outside_the_reach_are_ignored(self):
        start = SRC.index("void Other")
        reach = {"Other": (start, SRC.index("void Target"))}
        self.assertEqual(self.scope.statement_macros(SRC, reach, None), [("SET_MASK", 6)])
        self.assertEqual(self.scope.statement_macros("void F(void) {}\n", {"F": (0, 15)}, None), [])

    def test_pins_takes_the_function_first(self):
        self.assertEqual(self.scope.func_first_args(["a.c", "Foo", "pins"]), ["a.c", "pins", "Foo"])


if __name__ == "__main__":
    unittest.main()
