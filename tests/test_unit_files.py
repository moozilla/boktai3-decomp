"""Unit input lists distinguish protected assembly and remain safe to pipe."""
import contextlib
import io
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import unit_files


class UnitFiles(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        (self.root / "src").mkdir()
        self.funcs = [("First", 0x08000100), ("Veneer", 0x08000104),
                      ("Gap", 0x08000108), ("Last", 0x0800010C)]

    def write(self, name, source):
        (self.root / "src" / name).write_text(source)

    def run_main(self, *args):
        out, err = io.StringIO(), io.StringIO()
        with patch.object(unit_files, "ROOT", self.root), \
                patch.object(unit_files.progress, "load_functions", return_value=(self.funcs, {})), \
                patch.object(unit_files.progress, "load_asm_ranges",
                             return_value=[(0x08000104, 0x08000108)]), \
                contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            unit_files.main(list(args))
        return out.getvalue(), err.getvalue()

    def test_rom_order_and_source_aliases(self):
        self.write("z.c", "#define First sub_08000100\nvoid First(void) {}\n")
        self.write("a.c", "void Last(void) {}\n")
        files, missing = unit_files.covering_files(self.root, 0x08000100, 0x08000110, self.funcs)
        self.assertEqual([str(row[1]) for row in files], ["src/z.c", "src/a.c"])
        self.assertEqual(files[0][2], ["sub_08000100"])
        self.assertEqual(missing, self.funcs[1:3])

    def test_include_asm_is_covered_but_not_a_c_definition(self):
        self.write("unit.c", 'void First(void) {}\nINCLUDE_ASM("asm/nonmatching", Veneer);\n')
        files, missing = unit_files.covering_files(self.root, 0x08000100, 0x08000108, self.funcs)
        self.assertEqual(files[0][2], ["First", "Veneer"])
        self.assertEqual(missing, [self.funcs[1]])

    def test_nonmatching_draft_does_not_hide_a_gap(self):
        self.write("draft.c", '#ifdef NONMATCHING\nvoid Gap(void) {}\n#else\n'
                   'INCLUDE_ASM("asm/nonmatching", Gap);\n#endif\n')
        files, missing = unit_files.covering_files(self.root, 0x08000108, 0x0800010C, self.funcs)
        self.assertEqual(files[0][2], ["Gap"])
        self.assertEqual(missing, [self.funcs[2]])

    def test_default_output_separates_protected_assembly(self):
        self.write("unit.c", "void First(void) {}\n")
        out, err = self.run_main("08000100", "0800010C")
        self.assertEqual(out, "src/unit.c First\nunmatched: Gap (08000108)\n"
                         "protected assembly (symbols/progress_asm.csv): Veneer (08000104)\n")
        self.assertEqual(err, "")

    def test_paths_only_keeps_diagnostics_off_stdout(self):
        self.write("unit.c", "void First(void) {}\n")
        out, err = self.run_main("08000100", "0800010C", "--paths-only")
        self.assertEqual(out, "src/unit.c\n")
        self.assertIn("unmatched: Gap (08000108)", err)
        self.assertIn("protected assembly", err)
        self.assertNotIn("Last", out + err)

    def test_empty_or_reversed_range_is_rejected(self):
        for end in ("08000100", "080000FC"):
            with self.subTest(end=end), contextlib.redirect_stderr(io.StringIO()), \
                    self.assertRaises(SystemExit) as raised:
                unit_files.main(["08000100", end])
            self.assertEqual(raised.exception.code, 2)


if __name__ == "__main__":
    unittest.main()
