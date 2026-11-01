"""Regression coverage for the two static-inline declaration orders used by agbcc."""
from pathlib import Path
from tempfile import TemporaryDirectory
import unittest

from isolate import isolate


class IsolateHelpers(unittest.TestCase):
    def test_both_inline_orders_keep_definitions(self):
        text = """
static inline int first(int x) { return x + 1; }
inline static int second(int x) { return x + 2; }
int other(int x) { return first(x); }
int keep(int x) { return second(first(x)); }
"""
        with TemporaryDirectory() as temp:
            output = Path(temp) / "isolated.c"
            isolate(text, "keep", output, False)
            isolated = output.read_text()
        self.assertIn("return x + 1;", isolated)
        self.assertIn("return x + 2;", isolated)
        self.assertIn("return second(first(x));", isolated)
        self.assertNotIn("return first(x);", isolated)
