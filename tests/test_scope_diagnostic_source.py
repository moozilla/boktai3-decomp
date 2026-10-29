"""Diagnostic trimming must retain the inline bodies used by production."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class DiagnosticSource(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_multiline_inline_pointer_return_keeps_body(self):
        source = """struct Position { int x; };
struct Actor { struct Position motion; };
static inline struct Position *
MotionOf(struct Actor *actor)
{
    return &actor->motion;
}
int Target(struct Actor *actor)
{
    return MotionOf(actor)->x;
}
int Later(void) { return 99; }
"""
        trimmed = self.scope.diagnostic_source(source, ("Target",))
        self.assertIn("return &actor->motion;", trimmed)
        self.assertIn("return MotionOf(actor)->x;", trimmed)
        self.assertNotIn("Later", trimmed)
        self.assertEqual(trimmed.count("\n"), source[:source.index("int Later")].count("\n"))

    def test_inline_qualifier_does_not_leak_to_next_function(self):
        source = """static __inline__
int Helper(int value)
{
    return value + 1;
}
int Earlier(void)
{
    return 12345;
}
int Target(int value)
{
    return Helper(value);
}
"""
        trimmed = self.scope.diagnostic_source(source, ("Target",))
        self.assertIn("return value + 1;", trimmed)
        self.assertNotIn("return 12345;", trimmed)
        self.assertIn("int Earlier(void);", trimmed)
        self.assertIn("return Helper(value);", trimmed)


if __name__ == "__main__":
    unittest.main()
