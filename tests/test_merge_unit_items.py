import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from merge_unit import items, write_merge
from progress import _bodies


class FunctionBoundaries(unittest.TestCase):
    def test_write_keeps_destination_and_removes_inputs_without_git(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "first.c").write_text("first")
            (root / "second.c").write_text("second")
            with patch("merge_unit.ROOT", directory):
                write_merge("first.c", ["first.c", "second.c", "second.c"], "merged")
            self.assertEqual((root / "first.c").read_text(), "merged")
            self.assertFalse((root / "second.c").exists())

    def test_failed_destination_write_preserves_inputs(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "first.c").write_text("first")
            with patch("merge_unit.ROOT", directory), self.assertRaises(OSError):
                write_merge("missing/unit.c", ["first.c"], "merged")
            self.assertEqual((root / "first.c").read_text(), "first")

    def test_punctuation_in_multiline_comments_stays_in_its_function(self):
        source = """void Startup(void)
{
    /* The callee's (pointer) becomes the loader's argument.
     * Ignore { and unmatched parentheses here: (( */
    Load();
}
void SelectBase(void)
{
    register int base asm("r1");
}
"""
        bodies = _bodies(source)
        self.assertEqual(set(bodies), {"Startup", "SelectBase"})
        self.assertNotIn("SelectBase", bodies["Startup"][1])
        self.assertIn("register int base", bodies["SelectBase"][1])

    def test_strings_and_character_literals_keep_comment_markers_inert(self):
        declarations = ['const char *text = "/* )";', "char brace = '}';", "int next;"]
        self.assertEqual(items("\n".join(declarations)), declarations)

    def test_a_comment_inside_a_declaration_does_not_end_it(self):
        structure = "struct Holder\n/* The payload contains ( values. */\n{ int value; };"
        self.assertEqual(items(structure), [structure])
