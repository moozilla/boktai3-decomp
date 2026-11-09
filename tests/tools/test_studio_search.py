"""The Studio's search over everything (tools/studio/search.py)."""
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
for directory in (ROOT / 'tools', ROOT / 'tools' / 'assets', ROOT / 'tools' / 'studio'):
    sys.path.insert(0, str(directory))
import search

ATLAS = {
    'monster:9858': {'id': 'monster:9858', 'kind': 'monster', 'key': '9858', 'title': 'グール', 'jp': 'グール', 'path': 'Bestiary'},
    'scene:2719': {'id': 'scene:2719', 'kind': 'scene', 'key': '2719', 'title': 'Ghoul corridor', 'path': None, 'area': 'area:1'},
    'area:1': {'id': 'area:1', 'kind': 'area', 'key': '1', 'title': 'Sealed Prison (opening tutorial dungeon)',
               'location': 'Sealed Prison', 'path': 'Places/Sealed Prison'},
    'actor:F5EB': {'id': 'actor:F5EB', 'kind': 'actor', 'key': 'F5EB', 'title': 'Django', 'fname': 'Player_Create', 'path': 'Objects'},
}
CODE = {'categories': [{'name': 'Monsters', 'units': [{'name': 'Ghoul', 'functions': [
    ['Ghoul_Create', '0808E1D4', 100, 100, [], 'src/ghoul.c'], ['sub_0808E300', '0808E300', 20, 100, [], None]]}]}],
    'ram': [{'name': 'gGhoulCount', 'addr': '02001000', 'size': 4}]}


class SearchTests(unittest.TestCase):
    def setUp(self):
        for name, value in (('_atlas', lambda: ATLAS), ('_code', lambda: CODE), ('_procs', lambda: {12: 'spawn_ghoul'}),
                            ('_places', lambda: [{'name': 'Ancient Tree', 'areas': [4, 5], 'scenes': 14}]),
                            ('_resources', lambda: {'map': [['7EFB', 900]], 't92B3': [['081B', 512]]}),
                            ('_rom_leaves', lambda: [(0x0808E1D4, 0x0808E238, 'Ghoul_Create', ['Code', 'Ghoul'], '/code#0808E1D4'),
                                                     (0x085897F4, 0x0858A000, 'song 18', ['Sound (MP2K)', 'Songs'], 'song:18'),
                                                     (0x08700000, 0x08800000, 'free', [], None)])):
            context = patch.object(search, name, value)
            context.start()
            self.addCleanup(context.stop)

    def test_english_name_ranks_the_monster_first(self):
        hits = search.search('ghoul', english={9858: 'Ghoul'})
        self.assertEqual(hits[0], {'kind': 'monster', 'title': 'Ghoul', 'sub': 'グール', 'url': '/atlas#monster:9858'})
        kinds = [h['kind'] for h in hits]
        self.assertIn('unit', kinds)
        self.assertIn('function', kinds)
        self.assertIn('RAM', kinds)
        self.assertIn('procedure', kinds)

    def test_address_and_other_fields(self):
        self.assertEqual(search.search('0x0808e3')[0]['url'], '/code#0808E300')
        self.assertEqual(search.search('player_create')[0]['url'], '/atlas#actor:F5EB')

    def test_places_link_to_the_world_view(self):
        hit = search.search('ancient')[0]
        self.assertEqual(hit, {'kind': 'place', 'title': 'Ancient Tree', 'sub': '2 areas · 14 scenes',
                               'url': '/world#place=Ancient%20Tree'})

    def test_text_bank(self):
        strings = [{'id': 7, 'text': 'グールだ'}, {'id': 8, 'text': 'ほかの'}]
        hits = search.search('a ghoul', strings=strings, english={7: 'A ghoul!'})
        self.assertEqual([h['url'] for h in hits if h['kind'] == 'text'], ['/text#7'])

    def test_scene_number_and_resource_hashes(self):
        self.assertEqual(search.search('scene 2719')[0]['url'], '/atlas#scene:2719')
        self.assertEqual(search.search('2719')[0]['url'], '/atlas#scene:2719')
        self.assertEqual(search.search('ghoul corridor')[0]['sub'], 'scene 2719 · Sealed Prison')
        self.assertEqual(search.search('7efb')[0], {'kind': 'map', 'title': 'Map 7EFB', 'sub': 'resource map', 'url': '/gfx#maps/7EFB'})
        self.assertEqual(search.search('081b')[0]['url'], '/gfx#palettes/081B')

    def test_rom_address_finds_what_holds_it(self):
        self.assertEqual(search.search('0858980c')[0], {'kind': 'ROM', 'title': 'song 18', 'sub': 'Sound (MP2K) › Songs · +0x18',
                                                        'url': '/atlas#song:18'})
        self.assertEqual(search.search('0x0808E1E0')[0]['url'], '/code#0808E1D4')  # inside a function
        self.assertEqual(search.search('0808e1d4')[0]['kind'], 'function')  # its own address: no second hit
        self.assertEqual(len([h for h in search.search('0808e1d4') if h['url'] == '/code#0808E1D4']), 1)
        self.assertEqual(search.search('08700010')[0]['sub'], 'ROM · +0x10')
        self.assertEqual([h for h in search.search('08900000') if h['kind'] == 'ROM'], [])

    def test_empty_query(self):
        self.assertEqual(search.search('  '), [])


if __name__ == '__main__':
    unittest.main()
