import sys
import tempfile
from unittest.mock import patch
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from merge_unit import items, write_merge
from progress import _bodies


class FunctionBoundaries(unittest.TestCase):
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


class MergeWithoutGit(unittest.TestCase):
    def test_removes_inputs_preserving_output_without_invoking_git(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "first.c").write_text("void First(void) {}\n")
            (root / "unit.c").write_text("void Last(void) {}\n")
            with patch("merge_unit.ROOT", directory), patch("merge_unit.subprocess.run") as run:
                write_merge("unit.c", ["first.c", "./unit.c"], "merged\n", no_git=True)
            run.assert_not_called()
            self.assertFalse((root / "first.c").exists())
            self.assertEqual((root / "unit.c").read_text(), "merged\n")

    def test_output_failure_does_not_remove_inputs(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory, "first.c")
            source.write_text("original\n")
            with patch("merge_unit.ROOT", directory):
                with self.assertRaises(FileNotFoundError):
                    write_merge("missing/unit.c", ["first.c"], "merged\n", no_git=True)
            self.assertEqual(source.read_text(), "original\n")
