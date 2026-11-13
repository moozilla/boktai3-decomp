import sys
import unittest
import tempfile
from unittest.mock import patch
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from merge_unit import items
import merge_unit
from progress import _bodies


class FunctionBoundaries(unittest.TestCase):
    def test_write_works_without_git_and_keeps_output_when_also_an_input(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            first = root / "first.c"
            second = root / "second.c"
            first.write_text("void First(void) {}\n")
            second.write_text("void Second(void) {}\n")
            with patch.object(merge_unit, "ROOT", directory), patch.object(
                sys, "argv", ["merge_unit.py", "first.c", "first.c", "second.c", "--write"]
            ):
                merge_unit.main()
            self.assertIn("void First(void)", first.read_text())
            self.assertIn("void Second(void)", first.read_text())
            self.assertFalse(second.exists())

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
