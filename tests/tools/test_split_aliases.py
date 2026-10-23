"""C address aliases must preserve generated names and interior references."""
import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
import split


class SplitAliasTests(unittest.TestCase):
    def test_promoted_and_unpromoted_generated_names(self):
        for generated, definition in [('Panel_Update', 'sub_08004000'),
                                      ('sub_08004000', 'Panel_Update'),
                                      ('Panel_Update', 'Panel_Update')]:
            with self.subTest(generated=generated, definition=definition), \
                 tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                for folder in ('gen', 'symbols', 'src', 'build/asm'):
                    (root / folder).mkdir(parents=True)
                (root / 'symbols/functions.csv').write_text('08004000,Panel_Update\n')
                (root / 'symbols/build_extra_functions.csv').write_text('address,isa\n')
                (root / 'gen/code_sym.s').write_text(
                    f'\tthumb_func_start {generated}\n{generated}: @ 0x08004000\n'
                    '\tpush {lr}\n_08004002:\n\tbx lr\n'
                    '_08004004:\n\t.incbin "baserom.gba", 0x4004, 0x4\n')
                alias = '#define Panel_Update sub_08004000\n' if definition.startswith('sub_') else ''
                (root / 'src/panel.c').write_text(alias + 'void Panel_Update(void) {}\n')
                with patch.object(split, 'ROOT', directory), \
                     patch.object(split, 'function_nonentries', return_value=set()), \
                     contextlib.redirect_stdout(io.StringIO()), \
                     contextlib.redirect_stderr(io.StringIO()):
                    split.main()
                labels = (root / 'build/c_labels.ld').read_text()
                retained = ''.join(path.read_text() for path in
                                   (root / 'build/asm').glob('seg_*.s'))
                self.assertEqual(retained.count('.incbin "baserom.gba", 0x4004, 0x4'), 1)
                self.assertNotIn('_08004004 =', labels)
                self.assertIn(f'_08004002 = {generated}__addr + 0x2;', labels)
                if generated != definition:
                    self.assertIn(f'{generated} = {definition};', labels)
                    self.assertIn(f'{generated}__addr = {definition}__addr;', labels)
                else:
                    self.assertNotIn(f'{generated} = {generated};', labels)


if __name__ == '__main__':
    unittest.main()
