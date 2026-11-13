import contextlib
import io
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import unit_files


class UnitRangeInventory(unittest.TestCase):
    functions = [('First', 0x08001000), ('Middle', 0x08001004), ('Last', 0x08001008)]

    def source(self, root, name, text):
        path = Path(root, 'src', name)
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text)
        return path

    def test_files_are_ordered_by_rom_address(self):
        with tempfile.TemporaryDirectory() as root:
            self.source(root, 'z.c', 'void First(void) {}\n')
            self.source(root, 'a.c', 'void Middle(void) {}\nvoid Last(void) {}\n')
            files, missing, overlaps = unit_files.collect_units(root, self.functions, 0x08001000, 0x0800100C)
            self.assertEqual([str(row[1]) for row in files], ['src/z.c', 'src/a.c'])
            self.assertEqual(missing, [])
            self.assertEqual(overlaps, [])

    def test_both_boundaries_report_whole_file_overlap(self):
        with tempfile.TemporaryDirectory() as root:
            self.source(root, 'unit.c', '\n'.join('void ' + name + '(void) {}' for name, _ in self.functions))
            files, missing, overlaps = unit_files.collect_units(root, self.functions, 0x08001004, 0x08001008)
            self.assertEqual(files[0][2], ['Middle'])
            self.assertEqual(missing, [])
            self.assertEqual(overlaps, [(Path('src/unit.c'), ['First', 'Last'])])

    def test_address_names_and_assembly_placeholders_are_recognized(self):
        with tempfile.TemporaryDirectory() as root:
            self.source(root, 'unit.c', 'void sub_08001000(void) {}\nINCLUDE_ASM("asm/nonmatching", Middle);\n')
            files, missing, overlaps = unit_files.collect_units(root, self.functions, 0x08001000, 0x08001008)
            self.assertEqual(files[0][2], ['sub_08001000', 'Middle'])
            self.assertEqual(missing, ['Middle (08001004)'])
            self.assertEqual(overlaps, [])

    def run_cli(self, root, *args):
        out, err = io.StringIO(), io.StringIO()
        with mock.patch.object(unit_files, 'ROOT', root), \
             mock.patch.object(unit_files.progress, 'load_functions', return_value=(self.functions, None)), \
             mock.patch.object(sys, 'argv', ['unit_files.py', *args]), \
             contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            result = unit_files.main()
        return result, out.getvalue(), err.getvalue()

    def test_paths_only_outputs_only_complete_merge_inputs(self):
        with tempfile.TemporaryDirectory() as root:
            self.source(root, 'one.c', 'void First(void) {}\n')
            result, out, err = self.run_cli(root, '08001000', '08001004', '--paths-only')
            self.assertEqual((result, out, err), (0, 'src/one.c\n', ''))

    def test_paths_only_never_emits_a_partial_or_incomplete_list(self):
        with tempfile.TemporaryDirectory() as root:
            self.source(root, 'unit.c', 'void First(void) {}\nvoid Middle(void) {}\n')
            result, out, err = self.run_cli(root, '08001004', '08001008', '--paths-only')
            self.assertEqual(result, 1)
            self.assertEqual(out, '')
            self.assertIn('outside range in src/unit.c: First', err)
            result, out, err = self.run_cli(root, '08001000', '0800100C', '--paths-only')
            self.assertEqual(result, 1)
            self.assertEqual(out, '')
            self.assertIn('unmatched: Last (08001008)', err)

    def test_default_output_keeps_function_details_and_reports_overlap(self):
        with tempfile.TemporaryDirectory() as root:
            self.source(root, 'unit.c', 'void First(void) {}\nvoid Middle(void) {}\n')
            result, out, err = self.run_cli(root, '08001004', '08001008')
            self.assertEqual((result, out), (0, 'src/unit.c Middle\n'))
            self.assertIn('outside range', err)

    def test_empty_or_reversed_range_is_rejected(self):
        with tempfile.TemporaryDirectory() as root:
            for end in ['08001000', '08000000']:
                with self.subTest(end=end), self.assertRaises(SystemExit) as result:
                    self.run_cli(root, '08001000', end)
                self.assertEqual(result.exception.code, 2)
