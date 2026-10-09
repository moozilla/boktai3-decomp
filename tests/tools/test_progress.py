"""Regression tests for ROM-free progress counts and objdiff v2 JSON types."""
import json
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
import progress


class ProgressTests(unittest.TestCase):
    def test_inventory_and_named_address_alias(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            inventory = root / 'inventory.csv'
            progress.write_inventory(inventory, [progress.CODE_END - 12, progress.CODE_END - 8])
            symbols = root / 'functions.csv'
            symbols.write_text('addr,name\n0824DAEE,NamedFunction\n')
            funcs, sizes = progress.load_functions(inventory, symbols)
            (root / 'one.c').write_text('void sub_0824DAEE(void) { }\n'
                                      'static void helper(void) { }\n'
                                      'INCLUDE_ASM("asm/nonmatching", sub_0824DAF2);\n')
            # Duplicate declarations in another file must not inflate counts.
            (root / 'two.c').write_text('void NamedFunction(void) { }\n')
            done = progress.collect_done(funcs, root)
            self.assertEqual(done, {'NamedFunction'})
            report = json.loads(json.dumps(progress.make_report(funcs, sizes, done)))
            self.assertEqual(report['version'], 2)
            self.assertEqual(report['measures']['total_code'], '12')
            self.assertEqual(report['measures']['matched_code'], '4')
            self.assertEqual(report['measures']['total_functions'], 2)
            self.assertEqual(report['measures']['matched_functions'], 1)
            self.assertEqual(report['measures']['total_units'], 2)
            self.assertEqual(report['measures']['complete_units'], 1)
            self.assertEqual(report['measures']['matched_functions_percent'], 50)
            self.assertEqual(sum(int(u['measures']['total_code']) for u in report['units']), 12)
            self.assertEqual(sum(int(u['measures']['matched_code']) for u in report['units']), 4)
            self.assertIsInstance(report['units'][0]['functions'][0]['size'], str)
            self.assertIsInstance(report['units'][0]['functions'][0]['metadata']['virtual_address'], str)

    def test_reject_bad_inventory(self):
        for addresses in ([], [0x08000100, 0x08000100], [0x08000200, 0x08000100],
                          [0x07000000], [progress.CODE_END]):
            with self.subTest(addresses=addresses), self.assertRaises(ValueError):
                progress.validate_addresses(addresses)

    def test_reject_unknown_definition(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'unknown.c').write_text('void sub_08000104(void) { }\n')
            with self.assertRaisesRegex(ValueError, 'refresh inventory'):
                progress.collect_done([('sub_08000100', 0x08000100)], root)

    def test_export_only_function_start_addresses(self):
        with tempfile.TemporaryDirectory() as tmp:
            assembly = Path(tmp) / 'code.s'
            assembly.write_text('\tthumb_func_start sub_08000100\n'
                                'sub_08000100: @ 0x08000100\n\tmovs r0, #1\n'
                                'literal: @ 0x08000104\n\t.word 0xDEADBEEF\n'
                                '\tarm_func_start Named\nNamed: @ 0x08000108\n'
                                '\tnon_word_aligned_thumb_func_start Odd\nOdd: @ 0x0800010A\n')
            self.assertEqual(progress.assembly_addresses(assembly),
                             [0x08000100, 0x08000108, 0x0800010A])

    def test_reviewed_extra_boundary_is_not_double_counted(self):
        with tempfile.TemporaryDirectory() as tmp:
            assembly = Path(tmp) / 'code.s'
            assembly.write_text('\tthumb_func_start sub_08000100\n'
                                'sub_08000100: @ 0x08000100\n')
            extras = Path(tmp) / 'extras.csv'
            extras.write_text('address,evidence\n08000100,already known\n08000104,verified SDK entry\n')
            self.assertEqual(progress.inventory_addresses(assembly, extras), [0x08000100, 0x08000104])


if __name__ == '__main__':
    unittest.main()
