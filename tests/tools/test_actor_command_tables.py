"""Studio keeps dispatch indices and distinguishes direct references from ownership."""
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


class ActorCommandTableTests(unittest.TestCase):
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
        self.add_table('first', 0x08600000)

    def add_table(self, name, start, directory='monster_commands'):
        relative = f'data/{directory}/{name}.csv'
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('index,callback,name\n0,080837D5,OldLiftName\n'
                        '1,00000000,Unused\n2,08136EB1,OldRestoreName\n')
        self.entries.append(datafmt.Entry({
            'name': name, 'start': f'{start:08X}', 'end': f'{start + 12:08X}',
            'format': 'callback_array', 'file': relative, 'description': name + ' commands',
        }))

    def test_order_thumb_addresses_current_names_and_nulls(self):
        # A callback_array elsewhere need not be an actor command table.
        self.add_table('unrelated', 0x08600100, 'other_callbacks')
        tables = actors.command_tables()
        self.assertEqual(list(tables), ['08600000'])
        table = tables['08600000']
        self.assertEqual((table['size'], table['end']), (12, '0860000C'))
        self.assertEqual(table['source'], 'data/monster_commands/first.csv')
        self.assertEqual(table['entries'], [
            {'index': 0, 'callback': '080837D5', 'addr': '080837D4', 'name': 'Current_080837D4'},
            {'index': 1, 'callback': '00000000', 'addr': None, 'name': None},
            {'index': 2, 'callback': '08136EB1', 'addr': '08136EB0', 'name': 'Current_08136EB0'},
        ])

    def test_reference_index_uses_exact_table_starts_and_keeps_provenance(self):
        values = {0x08001000: [0x08600000, 0x08600000, 0x08600004],
                  0x08002000: [0, 0x08600000]}
        with patch.object(actors, 'literals', side_effect=values.__getitem__):
            refs = actors.command_tables_of([0x08002000, 0x08001000, 0x08001000])
        self.assertEqual(refs, [{'table': '08600000', 'in': [
            {'addr': '08001000', 'name': 'Current_08001000'},
            {'addr': '08002000', 'name': 'Current_08002000'},
        ]}])

    def test_actor_links_do_not_inherit_a_nearby_callees_table(self):
        self.add_table('loader', 0x08600020)
        self.add_table('neighbor', 0x08600040)
        values = {0x08001000: [0x08600000], 0x08002000: [0x08600020],
                  0x08002100: [0x08600040]}
        with patch.object(actors, 'code', return_value=({}, {'Nearby': 0x08002100})), \
             patch.object(actors, 'registry', return_value={'1234': {'callback': '08001001'}}), \
             patch.object(actors, 'procs', return_value={}), \
             patch.object(actors, 'proc_scenes', return_value={}), \
             patch.object(actors, 'bank', return_value={}), \
             patch.object(actors.gclnames, 'load', return_value={}), \
             patch.object(actors, 'loader_of', return_value=0x08002000), \
             patch.object(actors, 'calls', side_effect=lambda a: ['Nearby'] if a == 0x08002000 else []), \
             patch.object(actors, 'sprites_of', return_value=[]), \
             patch.object(actors, 'literals', side_effect=values.__getitem__):
            actor = actors.actors()[0]
        self.assertEqual([ref['table'] for ref in actor['command_tables']], ['08600000', '08600020'])
        self.assertEqual(actor['command_tables'][1]['in'][0]['addr'], '08002000')

    def test_rom_blocks_mark_only_the_decoded_extent(self):
        self.add_table('second', 0x08600100)
        blocks = atlas.command_table_blocks()
        self.assertEqual([(b['s'], b['n']) for b in blocks], [(0x08600000, 12), (0x08600100, 12)])
        self.assertEqual(sum(b['n'] for b in blocks), 24)
        self.assertTrue(all(b['u'] == 'decoded' and b['k'] == 'data' for b in blocks))
        self.assertEqual(blocks[1]['r'], '/actors#commands:08600100')

    def test_rom_tree_does_not_double_count_tables_as_surrounding_data(self):
        self.add_table('second', 0x08600100)
        segments = {'size': 0x1000000, 'items': [],
                    'regions': [[0x08600000, 0x08600200, 'data', 'Actor tables']]}
        sound = SimpleNamespace(load=lambda: {
            'table': {'addr': '08200000', 'size': 4},
            'voicegroups': [], 'samples': [], 'songs': [],
        }, titles=lambda: {})
        with patch.object(atlas, 'rom_load', return_value=segments), \
             patch.object(atlas, 'notes', return_value={}), \
             patch.object(atlas, 'load', return_value={'entities': {}, 'funcs': {}}), \
             patch.dict(sys.modules, {'sound': sound}):
            tree = atlas.rom_tree()
        data = [block for block in tree['children'] if block['k'] == 'data']
        self.assertEqual(sum(block['n'] for block in data), 0x200)
        commands = next(block for block in data if 'c' in block)
        self.assertEqual(commands['n'], 24)
        self.assertTrue(all(block.get('u') == 'located' for block in data if 'c' not in block))


if __name__ == '__main__':
    unittest.main()
