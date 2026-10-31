"""Studio separates numeric parameter rows from callback dispatch tables."""
import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
for directory in (ROOT / 'tools', ROOT / 'tools' / 'assets', ROOT / 'tools' / 'studio'):
    sys.path.insert(0, str(directory))
import actors
import atlas
import datafmt
from datafmt import actor_marker_table as codec


class ActorMarkerStudioTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.entries = []
        for target, name, value in ((actors, 'ROOT', str(self.root)),
                                    (datafmt, 'ROOT', str(self.root))):
            context = patch.object(target, name, value)
            context.start()
            self.addCleanup(context.stop)
        context = patch.dict(actors._CACHE, {}, clear=True)
        context.start()
        self.addCleanup(context.stop)
        context = patch.object(actors, 'load_manifest', return_value=self.entries)
        context.start()
        self.addCleanup(context.stop)
        context = patch.object(actors, 'fname', side_effect=lambda a: f'Current_{a:08X}')
        context.start()
        self.addCleanup(context.stop)
        for start, table in codec.TABLES.items():
            relative = f'data/actor_marker/{start:08X}.csv'
            path = self.root / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            text = ','.join(table['columns']) + '\n'
            for i in range(18):
                text += ','.join(map(str, (*divmod(i, 9), *([0] * len(table['format']))))) + '\n'
            path.write_text(text)
            self.entries.append(datafmt.Entry({
                'name': f'marker_{start:08X}', 'start': f'{start:08X}',
                'end': f'{table["end"]:08X}', 'format': 'actor_marker_table',
                'file': relative, 'description': 'Marker parameter table',
            }))

    def test_numeric_schema_preserves_rows_and_current_consumer_names(self):
        tables = actors.numeric_tables()
        self.assertEqual(len(tables), 3)
        frames = tables['0824E6D4']
        self.assertEqual(frames['source'], 'data/actor_marker/0824E6D4.csv')
        self.assertEqual(frames['columns'], ['facing', 'fragment', 'frame'])
        self.assertEqual(frames['entries'][-1], {'facing': 1, 'fragment': 8, 'frame': 0})
        self.assertEqual(frames['consumers'], [{'addr': '08180CE0', 'name': 'Current_08180CE0'}])
        self.assertEqual(tables['0824E818']['consumers'],
                         [{'addr': '08181514', 'name': 'Current_08181514'}])
        self.assertEqual(actors.command_tables(), {})

    def test_rom_blocks_mark_only_the_180_decoded_bytes(self):
        blocks = atlas.numeric_table_blocks()
        self.assertEqual([(b['s'], b['n']) for b in blocks],
                         [(0x0824E6D4, 18), (0x0824E6E6, 18), (0x0824E818, 144)])
        self.assertEqual(sum(b['n'] for b in blocks), 180)
        self.assertTrue(all(b['u'] == 'decoded' and b['k'] == 'data' for b in blocks))
        self.assertEqual(blocks[-1]['r'], '/actors#tables:0824E818')
        for a, b in zip(blocks, blocks[1:]):
            self.assertLessEqual(a['s'] + a['n'], b['s'])

    def test_rom_tree_keeps_intervening_data_undecoded(self):
        start, end = 0x0824E6D4, 0x0824E8A8
        segments = {'size': 0x1000000, 'items': [],
                    'regions': [[start, end, 'data', 'Marker parameters']]}
        sound = SimpleNamespace(load=lambda: {
            'table': {'addr': '08200000', 'size': 4},
            'voicegroups': [], 'samples': [], 'songs': [],
        }, titles=lambda: {})
        with patch.object(atlas, 'rom_load', return_value=segments), \
             patch.object(atlas, 'notes', return_value={}), \
             patch.object(atlas, 'load', return_value={'entities': {}, 'funcs': {}}), \
             patch.object(atlas, 'player_animation_blocks', return_value=[]), \
             patch.dict(sys.modules, {'sound': sound}):
            tree = atlas.rom_tree()
        blocks = [block for block in tree['children'] if block['k'] == 'data']
        self.assertEqual(sum(block['n'] for block in blocks), end - start)
        self.assertEqual(sum(block['n'] for block in blocks if block['u'] == 'decoded'), 180)
        gaps = [block for block in blocks if block['u'] == 'located']
        self.assertEqual([(block['s'], block['n']) for block in gaps],
                         [(0x0824E6F8, 0x120)])
        for a, b in zip(blocks, blocks[1:]):
            self.assertLessEqual(a['s'] + a['n'], b['s'])


if __name__ == '__main__':
    unittest.main()
