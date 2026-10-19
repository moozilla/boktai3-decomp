"""gclroles names scene procedures from how readable lines refer to them."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools' / 'assets'))
import gclroles


class ReferenceTests(unittest.TestCase):
    def test_keyword_list_and_positional_references(self):
        line = ('SceneMessages_ScriptStart(scene=198, step=0, ids=[0xde69, 0x9786], '
                'scripts=[proc_3821, proc_1675], onfinish=proc_3763)  # proc_9 in a comment')
        self.assertEqual(gclroles.references(line), [
            (3821, 'SceneMessages_ScriptStart', 'scripts'),
            (1675, 'SceneMessages_ScriptStart', 'scripts'),
            (3763, 'SceneMessages_ScriptStart', 'onfinish'),
        ])
        self.assertEqual(gclroles.references('entrance_at_box_911(box=0xcd22, facing=1, 2, 0, 57, proc_3838)'),
                         [(3838, 'entrance_at_box_911', '')])

    def test_calls_strings_and_log_lines_are_not_references(self):
        self.assertEqual(gclroles.references('proc_3872()'), [])
        self.assertEqual(gclroles.references('print("proc_12", x=proc_5)'), [(5, 'print', 'x')])
        self.assertEqual(gclroles.references('# log: proc_12'), [])

    def test_body_roles(self):
        named = {'door': 934, 'entrance_at_box_911': 911}
        files = {934: 'doors', 911: 'doors'}
        role = gclroles._body_role(['load_stage(map=0xc2a0, tileset=0xf81d, hp=0x50ad)'], named, files)
        self.assertEqual(role[0], 'stage')
        role = gclroles._body_role(['entrance_at_box_911(box=1)', 'door(box=1, scene=proc_2, entrance=7)'],
                                   named, files)
        self.assertEqual(role[0], 'doors')
        role = gclroles._body_role(['door_transition(scene=proc_3107, entrance=92, 52)'], named, files)
        self.assertEqual(role[0], 'exit_to_proc_3107')
        self.assertIsNone(gclroles._body_role(['proc_77(1, 2)'], named, files))


if __name__ == '__main__':
    unittest.main()
