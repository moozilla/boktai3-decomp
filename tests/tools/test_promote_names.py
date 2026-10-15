"""Integration also normalizes calls to names promoted before a worker's base."""
import csv
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
import promote_names


class NamePromotionTests(unittest.TestCase):
    def test_existing_names_and_new_aliases_are_rewritten_together(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for name in ('src', 'include', 'symbols', 'gen'):
                (root / name).mkdir()
            table = root / 'symbols/functions.csv'
            table.write_text('addr,name,mode,confidence,source,notes\n'
                             '08000100,Existing_Reset,thumb,low,src-alias,\n')
            (root / 'gen/code.s').write_text(
                '\tthumb_func_start sub_08000100\n'
                '\tthumb_func_start sub_08000200\n')
            source = root / 'src/worker.c'
            source.write_text('void sub_08000100(void);\n'
                              '#define Worker_Run sub_08000200\n'
                              'void Worker_Run(void) { sub_08000100(); }\n')
            header = root / 'include/worker.h'
            header.write_text('void sub_08000100(void);\n')
            ram = root / 'symbols/ram.ld'
            ram.write_text('Existing_Reset = sub_08000100;\n'
                           'Worker_Run = sub_08000200;\n')
            with patch.object(promote_names, 'ROOT', directory), \
                 patch.object(promote_names, 'CSV', str(table)), \
                 patch.object(sys, 'argv', ['promote_names.py']):
                promote_names.main()
            self.assertEqual(source.read_text(),
                             'void Existing_Reset(void);\n'
                             'void Worker_Run(void) { Existing_Reset(); }\n')
            self.assertEqual(header.read_text(), 'void Existing_Reset(void);\n')
            self.assertEqual(ram.read_text(), '')
            with table.open() as stream:
                rows = list(csv.DictReader(stream))
            self.assertEqual([row['name'] for row in rows], ['Existing_Reset', 'Worker_Run'])


if __name__ == '__main__':
    unittest.main()
