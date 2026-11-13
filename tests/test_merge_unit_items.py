import sys
import unittest
import tempfile
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


class MergeWrites(unittest.TestCase):
    def test_write_preserves_output_that_is_also_an_input(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory, "unit.c")
            source = Path(directory, "single.c")
            output.write_text("old")
            source.write_text("input")
            write_merge(str(output), [str(output), str(source)], "merged")
            self.assertEqual(output.read_text(), "merged")
            self.assertFalse(source.exists())

    def test_failed_output_write_keeps_inputs(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory, "single.c")
            source.write_text("input")
            with self.assertRaises(OSError):
                write_merge(str(Path(directory, "absent", "unit.c")), [str(source)], "merged")
            self.assertEqual(source.read_text(), "input")
