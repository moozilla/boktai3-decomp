"""Cursor updates retain a single death only when increment and mask are separate."""
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import permute  # noqa: E402,F401
sys.path.insert(0, permute.PERMUTER)
import spellings  # noqa: E402
from perm_pycparser import c_parser  # noqa: E402
from rewrites import text  # noqa: E402


class SplitMaskedUpdate(unittest.TestCase):
    def variants(self, body, declarations="unsigned int i, mask;"):
        source = "void f(void) { " + declarations + body + " }"
        fn = c_parser.CParser().parse(source).ext[0]
        before = text(fn)
        variants = list(spellings.sp_split_masked_update(fn))
        self.assertEqual(text(fn), before)
        return [text(node) for _, node in variants]

    def test_splits_cursor_and_retains_statement_order(self):
        variants = self.variants("i = (i + 1) & mask; i = (i - 2) & 1023;")
        self.assertEqual(len(variants), 2)
        self.assertIn("i += 1;\n  i &= mask;", variants[0])
        self.assertIn("i -= 2;\n  i &= 1023;", variants[1])

    def test_rejects_mask_that_reads_old_cursor(self):
        self.assertEqual(self.variants("i = (i + 1) & i;"), [])

    def test_rejects_calls_pointers_and_nonconstant_updates(self):
        for stmt in ["i = (i + mask) & 1023;", "i = (i + 1) & get_mask();",
                     "*p = (*p + 1) & mask;"]:
            self.assertEqual(self.variants(stmt), [])

    def test_rejects_volatile_narrow_signed_or_shadowed_cursors(self):
        for declarations in ["volatile unsigned int i; unsigned int mask;",
                             "unsigned short i; unsigned int mask;",
                             "int i; unsigned int mask;"]:
            self.assertEqual(self.variants("i = (i + 1) & mask;", declarations), [])
        self.assertEqual(self.variants("{ unsigned int i; i = (i + 1) & mask; }"), [])

    def test_only_plain_statement_assignments(self):
        self.assertEqual(self.variants("if ((i = (i + 1) & mask)) return;"), [])


if __name__ == "__main__":
    unittest.main()
