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
    'scene:2719': {'id': 'scene:2719', 'kind': 'scene', 'key': '2719', 'title': 'Ghoul corridor', 'path': 'Places'},
    'actor:F5EB': {'id': 'actor:F5EB', 'kind': 'actor', 'key': 'F5EB', 'title': 'Django', 'fname': 'Player_Create', 'path': 'Objects'},
}
CODE = {'categories': [{'name': 'Monsters', 'units': [{'name': 'Ghoul', 'functions': [
    ['Ghoul_Create', '0808E1D4', 100, 100, [], 'src/ghoul.c'], ['sub_0808E300', '0808E300', 20, 100, [], None]]}]}],
    'ram': [{'name': 'gGhoulCount', 'addr': '02001000', 'size': 4}]}


class SearchTests(unittest.TestCase):
    def setUp(self):
        for name, value in (('_atlas', lambda: ATLAS), ('_code', lambda: CODE), ('_procs', lambda: {12: 'spawn_ghoul'})):
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

    def test_text_bank(self):
        strings = [{'id': 7, 'text': 'グールだ'}, {'id': 8, 'text': 'ほかの'}]
        hits = search.search('a ghoul', strings=strings, english={7: 'A ghoul!'})
        self.assertEqual([h['url'] for h in hits if h['kind'] == 'text'], ['/text#7'])

    def test_empty_query(self):
        self.assertEqual(search.search('  '), [])


if __name__ == '__main__':
    unittest.main()
