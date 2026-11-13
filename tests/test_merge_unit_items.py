import sys
import unittest
import tempfile
import contextlib
import io
from unittest import mock
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from merge_unit import items, merge, main, write_merge
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

    def test_unlisted_existing_output_is_not_overwritten(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory, "unit.c")
            source = Path(directory, "single.c")
            output.write_text("unrelated unit")
            source.write_text("input")
            with self.assertRaisesRegex(ValueError, "existing output"):
                write_merge(str(output), [str(source)], "merged")
            self.assertEqual(output.read_text(), "unrelated unit")
            self.assertEqual(source.read_text(), "input")

    def test_duplicate_input_paths_are_rejected_before_writing(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory, "unit.c")
            source = Path(directory, "single.c")
            source.write_text("input")
            alias = str(source.parent) + "/./single.c"
            with self.assertRaisesRegex(ValueError, "distinct files"):
                write_merge(str(output), [str(source), alias], "merged")
            self.assertFalse(output.exists())
            self.assertEqual(source.read_text(), "input")

    def test_output_alias_of_input_is_preserved(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory, "unit.c")
            alias = Path(directory, "alias.c")
            source = Path(directory, "single.c")
            output.write_text("old")
            alias.symlink_to(output)
            source.write_text("input")
            write_merge(str(alias), [str(output), str(source)], "merged")
            self.assertEqual(output.read_text(), "merged")
            self.assertTrue(alias.is_symlink())
            self.assertFalse(source.exists())


class MergeCompilerSettings(unittest.TestCase):
    def merge_sources(self, first, second):
        with tempfile.TemporaryDirectory() as directory:
            paths = [Path(directory, "first.c"), Path(directory, "second.c")]
            paths[0].write_text(first + "\nvoid First(void) {}\n")
            paths[1].write_text(second + "\nvoid Second(void) {}\n")
            return merge([str(path) for path in paths])

    def test_mixed_compilers_are_reported_in_either_order(self):
        for first, second in [("", "// COMPILER: old_agbcc"),
                              ("// COMPILER: old_agbcc", "")]:
            with self.subTest(first=first):
                _, conflicts = self.merge_sources(first, second)
                self.assertEqual(conflicts[0][0][0], "compiler")

    def test_implicit_and_explicit_default_compiler_agree(self):
        _, conflicts = self.merge_sources("", "// COMPILER: agbcc")
        self.assertEqual(conflicts, [])

    def test_matching_legacy_compilers_remain_supported(self):
        text, conflicts = self.merge_sources("// COMPILER: old_agbcc",
                                             "// COMPILER: old_agbcc")
        self.assertEqual(conflicts, [])
        self.assertIn("// COMPILER: old_agbcc", text)
        self.assertIn("void First", text)
        self.assertIn("void Second", text)

    def test_effective_flags_must_agree(self):
        _, conflicts = self.merge_sources("", "// CFLAGS: -O1 -mthumb-interwork")
        self.assertEqual(conflicts[0][0][0], "compiler flags")
        _, conflicts = self.merge_sources("", "// CFLAGS: -O2 -mthumb-interwork")
        self.assertEqual(conflicts, [])

    def test_conflict_does_not_write_output_or_remove_inputs(self):
        with tempfile.TemporaryDirectory() as directory:
            first = Path(directory, "first.c")
            second = Path(directory, "second.c")
            output = Path(directory, "merged.c")
            first_text = "void First(void) {}\n"
            second_text = "// COMPILER: old_agbcc\nvoid Second(void) {}\n"
            first.write_text(first_text)
            second.write_text(second_text)
            argv = ["merge_unit.py", str(output), str(first), str(second), "--write"]
            with mock.patch.object(sys, "argv", argv), contextlib.redirect_stdout(io.StringIO()):
                with self.assertRaises(SystemExit) as result:
                    main()
            self.assertEqual(result.exception.code, 1)
            self.assertFalse(output.exists())
            self.assertEqual(first.read_text(), first_text)
            self.assertEqual(second.read_text(), second_text)
