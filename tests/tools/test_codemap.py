"""The Studio's code explorer index (tools/studio/codemap.py)."""
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
for directory in (ROOT / 'tools', ROOT / 'tools' / 'assets', ROOT / 'tools' / 'studio'):
    sys.path.insert(0, str(directory))
import codemap


def asm(name, addr, *body):
    return {'name': name, 'mode': 'thumb', 'lines': [f'{name}: @ 0x{addr:08X}', *body]}


FUNCS = {
    0x08001000: asm('Foo_Update', 0x08001000,
                    '\tldr r0, _08001010 @ =0x0300523C',
                    '\tldr r1, _08001014 @ =0x02000014',
                    '\tldr r2, _08001018 @ =Foo_OnDone',
                    '\tldr r3, _0800101C @ =0x086149C8',
                    '\tbl Foo_Helper'),
    0x08001020: asm('Foo_Helper', 0x08001020, '\tldr r0, _08001030 @ =0x08001041', '\tbx lr'),
    0x08001040: asm('Foo_OnDone', 0x08001040, '\tbx lr'),
}
NAMES = {f['name']: a for a, f in FUNCS.items()}
RAM = [{'addr': '02000010', 'name': 'gScroll', 'size': 0x10}, {'addr': '0300523C', 'name': 'gPause', 'size': 4}]
DATA = [[{'addr': '086149C4', 'name': 'gSine', 'size': 0x280}], [], []]


class XrefTests(unittest.TestCase):
    def test_literal_pools_resolve_ram_fields_data_and_callbacks(self):
        x = codemap.xrefs(FUNCS, NAMES, RAM, DATA)
        self.assertEqual(x[0x08001000]['ram'], {'gPause': [0], 'gScroll': [4]})
        self.assertEqual(x[0x08001000]['data'], ['gSine'])
        self.assertEqual(x[0x08001000]['ptrs'], [0x08001040])
        # a Thumb function address in a pool (odd) is a pointer to that function
        self.assertEqual(x[0x08001020]['ptrs'], [0x08001040])
        self.assertNotIn(0x08001040, x)

    def test_address_outside_every_object_stays_raw(self):
        funcs = {0x08002000: asm('Bar', 0x08002000, '\tldr r0, _0 @ =0x02030000')}
        self.assertEqual(codemap.xrefs(funcs, {'Bar': 0x08002000}, RAM, DATA)[0x08002000]['ram'], {'02030000': [0]})


class IndexTests(unittest.TestCase):
    def setUp(self):
        tree = {'version': 1, 'files': ['src/foo.c'], 'categories': [
            {'name': 'Engine core', 'units': [{'name': 'Foo', 'functions': [
                ['Foo_Update', '08001000', 32, 100, 0, 0],
                ['Foo_Helper', '08001020', 32, 80.0, 1, 0],
                ['Foo_OnDone', '08001040', 4, 100, 0, -1]]}]}]}
        report = {'measures': {'total_code': '68', 'complete_code': '36', 'complete_code_percent': 52.9,
                               'total_functions': 3},
                  'categories': [{'name': 'Engine core', 'measures': {'total_code': '68'}}]}
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        for target, value in (('_progress', lambda: (tree, report, {'Foo_Helper': ['pin']})),
                              ('_ram', lambda: RAM), ('_data', lambda: DATA),
                              ('CACHE', str(Path(temporary.name) / 'codemap.json'))):
            context = patch.object(codemap, target, value)
            context.start()
            self.addCleanup(context.stop)
        import actors
        context = patch.object(actors, 'code', lambda: (FUNCS, NAMES))
        context.start()
        self.addCleanup(context.stop)
        context = patch.dict(codemap._INDEX, {'mtime': None, 'data': None})
        context.start()
        self.addCleanup(context.stop)

    def test_build_places_functions_in_the_report_tree(self):
        ix = json.loads(json.dumps(codemap.build()))
        self.assertEqual(ix['missing'], [])
        self.assertEqual(ix['functions']['08001020']['kinds'], ['pin'])
        self.assertEqual(ix['functions']['08001040']['file'], 'src/fn/sub_08001040.c')
        self.assertEqual(ix['calls'], {'08001000': ['08001020']})
        self.assertEqual(sorted(ix['ptr_users']['08001040']), ['08001000', '08001020'])
        self.assertEqual(ix['ram_users']['gPause'], ['08001000'])
        self.assertEqual(ix['concession_kinds'], {'pin': 1})

    def test_summary_and_lookups(self):
        s = codemap.summary()
        fns = s['categories'][0]['units'][0]['functions']
        self.assertEqual([f[0] for f in fns], ['Foo_Update', 'Foo_Helper', 'Foo_OnDone'])
        self.assertEqual(next(r for r in s['ram'] if r['name'] == 'gPause')['users'], 1)
        self.assertEqual(codemap.find('Foo_Helper'), '08001020')
        self.assertEqual(codemap.find('0x08001025'), '08001020')  # inside a function
        self.assertEqual(codemap.find('sub_08001040'), '08001040')
        self.assertIsNone(codemap.find('08009999'))
        r = codemap.ram_object('gScroll')
        self.assertEqual(r['region'], 'EWRAM')
        self.assertEqual(r['users'][0]['offsets'], [4])
        self.assertIsNone(codemap.ram_object('gNothing'))

    def test_missing_disassembly_degrades_to_the_tree(self):
        import actors

        def no_disasm():
            raise FileNotFoundError('gen/code.s')
        with patch.object(actors, 'code', no_disasm):
            ix = codemap.build()
        self.assertEqual(len(ix['functions']), 3)
        self.assertEqual(ix['xrefs'], {})
        self.assertTrue(any('gen/code.s' in m for m in ix['missing']))


class StaleIndexTests(unittest.TestCase):
    def test_a_stale_index_is_served_while_the_new_one_builds(self):
        old, started = {'functions': {}}, []
        with patch.dict(codemap._INDEX, {'mtime': 1.0, 'data': old, 'checked': 0}), \
                patch.object(codemap, '_newest', return_value=2.0), \
                patch.object(codemap, '_rebuild_in_background', side_effect=started.append):
            self.assertIs(codemap.load(), old)
        self.assertEqual(started, [2.0])

    def test_wait_blocks_for_the_new_index(self):
        new = {'functions': {'x': 1}}
        with patch.dict(codemap._INDEX, {'mtime': 1.0, 'data': {}, 'checked': 0}), \
                patch.object(codemap, '_newest', return_value=2.0), \
                patch.object(codemap, '_rebuild', return_value=new):
            self.assertIs(codemap.load(wait=True), new)


class CommittedProgressTests(unittest.TestCase):
    def test_fallback_reads_progress_json(self):
        tree, report, kinds = codemap._committed_progress()
        self.assertTrue(report['stale'])
        self.assertEqual(report['measures']['total_functions'],
                         sum(len(u['functions']) for c in tree['categories'] for u in c['units']))
        self.assertTrue(all(k == ['concession'] for k in kinds.values()))


if __name__ == '__main__':
    unittest.main()
