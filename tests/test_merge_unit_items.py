import sys
import unittest
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from merge_unit import items, write_merge
from progress import _bodies


class FunctionBoundaries(unittest.TestCase):
    def test_write_merge_preserves_output_input_and_needs_no_git_repository(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory) / "unit.c"
            other = Path(directory) / "other.c"
            output.write_text("void First(void) {}")
            other.write_text("void Second(void) {}")
            merged = output.read_text() + "\n" + other.read_text()
            write_merge(str(output), [str(output), str(other)], merged)
            self.assertEqual(output.read_text(), merged)
            self.assertFalse(other.exists())

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
