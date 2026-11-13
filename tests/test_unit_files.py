import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import unit_files


class UnitRanges(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        (self.root / "src").mkdir()
        self.funcs = [("Before", 0x100), ("First", 0x110), ("Second", 0x120), ("After", 0x130)]

    def source(self, name, text):
        (self.root / "src" / name).write_text(text)

    def test_files_are_in_rom_order_and_end_is_exclusive(self):
        self.source("a.c", "void Second(void) {}\nvoid After(void) {}\n")
        self.source("z.c", "void First(void) {}\n")
        units, missing = unit_files.collect_units(self.root, 0x110, 0x130, self.funcs)
        self.assertEqual([str(unit.path) for unit in units], ["src/z.c", "src/a.c"])
        self.assertEqual(units[1].functions, ("Second",))
        self.assertEqual(units[1].outside, (("After", 0x130),))
        self.assertEqual(missing, [])

    def test_both_boundaries_and_address_aliases_are_reported(self):
        self.source("merged.c", "void Before(void) {}\nvoid sub_00000110(void) {}\nvoid After(void) {}\n")
        units, missing = unit_files.collect_units(self.root, 0x110, 0x120, self.funcs)
        self.assertEqual(units[0].outside, (("Before", 0x100), ("After", 0x130)))
        self.assertEqual(missing, [])

    def test_assembly_fallback_remains_unmatched(self):
        self.source("mixed.c", 'void First(void) {}\nINCLUDE_ASM("asm/nonmatching", Second);\n')
        units, missing = unit_files.collect_units(self.root, 0x110, 0x130, self.funcs)
        self.assertEqual(units[0].functions, ("First", "Second"))
        self.assertEqual(missing, [("Second", 0x120)])

    def test_strict_rejection_has_no_mergeable_stdout(self):
        self.source("merged.c", "void Before(void) {}\nvoid First(void) {}\n")
        out, err = io.StringIO(), io.StringIO()
        with patch.object(unit_files, "ROOT", self.root), patch.object(unit_files.progress, "load_functions", return_value=(self.funcs, {})):
            with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
                status = unit_files.main(["110", "120", "--strict"])
        self.assertEqual(status, 1)
        self.assertEqual(out.getvalue(), "")
        self.assertIn("outside range: src/merged.c: Before (00000100)", err.getvalue())

    def test_invalid_range_is_rejected(self):
        with self.assertRaises(ValueError):
            unit_files.collect_units(self.root, 0x120, 0x110, self.funcs)


if __name__ == "__main__":
    unittest.main()
