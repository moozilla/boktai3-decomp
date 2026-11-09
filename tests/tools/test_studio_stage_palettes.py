"""Stage palettes learned from captures (tools/studio/live.py remember_stage_palette)."""
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
for directory in (ROOT / 'tools', ROOT / 'tools' / 'studio'):
    sys.path.insert(0, str(directory))
import live


class StagePaletteTests(unittest.TestCase):
    def test_a_known_palette_leaves_the_file_alone(self):
        with tempfile.TemporaryDirectory() as d, patch.object(live, 'STAGE_PALS', os.path.join(d, 'p.csv')):
            Path(live.STAGE_PALS).write_text('map,palette,evidence\nAAAA,1111,seen in the guide\n')
            os.utime(live.STAGE_PALS, (1, 1))
            live.remember_stage_palette('AAAA', 'ts', '1111')  # viewing a capture again: no write
            self.assertEqual(os.path.getmtime(live.STAGE_PALS), 1)
            self.assertIn('seen in the guide', Path(live.STAGE_PALS).read_text())
            live.remember_stage_palette('BBBB', 'ts', '2222')
            self.assertEqual([r['map'] for r in live.stage_palettes()], ['AAAA', 'BBBB'])


if __name__ == '__main__':
    unittest.main()
