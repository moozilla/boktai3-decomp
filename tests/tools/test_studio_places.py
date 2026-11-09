"""The World view's places (tools/studio/places.py)."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools' / 'studio'))
import places


def area(n, label, scenes, enemies=None):
    return {'area': n, 'label': label, 'scenes': scenes, 'enemies': enemies or {}}


class PlaceTests(unittest.TestCase):
    def test_labels_group_areas_into_places_in_story_order(self):
        w = {'areas': [area(0, 'Dark Castle A (hall)', [50, 51]), area(1, 'Ancient Tree (entrance)', [10, 11], {'bat': 2}),
                       area(2, 'Ancient Tree (side room)', [12], {'bat': 1, 'slime': 1}), area(3, 'Dark Castle A', [52]),
                       area(4, 'Ancient Tree optional challenge / Black Knight', [13]), area(5, None, [99]),
                       area(6, 'Dark Castle (gate)', [40])],
             'scenes': {10: {'shot': False}, 11: {'shot': True}, 50: {'shot': True}}}
        ps = places.assign(w)
        self.assertEqual([p['name'] for p in ps], ['Ancient Tree', 'Dark Castle', 'Dark Castle A'])
        tree = ps[0]
        self.assertEqual(tree['areas'], [1, 2, 4])
        self.assertEqual(tree['enemies'], {'bat': 3, 'slime': 1})
        self.assertEqual(tree['shot'], 11)  # the first captured scene of its biggest area
        self.assertIsNone(ps[1]['hue'])  # one small area: no color of its own
        self.assertEqual([a['detail'] for a in w['areas']],
                         ['hall', 'entrance', 'side room', '', 'optional challenge / Black Knight', '', 'gate'])
        self.assertIsNone(w['areas'][5]['place'])

    def test_story_order_comes_before_scene_ids(self):
        w = {'areas': [area(0, 'Menu / bike setup', [5]), area(1, 'Prologue', [90]), area(2, 'Sealed Prison', [20])],
             'scenes': {}}
        w['areas'][1]['story_order'] = 1
        w['areas'][2]['story_order'] = 2  # 0 or missing: unknown, after the rest
        self.assertEqual([p['name'] for p in places.assign(w)], ['Prologue', 'Sealed Prison', 'Menu'])


if __name__ == '__main__':
    unittest.main()
