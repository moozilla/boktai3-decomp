"""Which script procedures show a text-bank string (tools/studio/serve.py text_uses)."""
import sys
import types
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
for directory in (ROOT / 'tools', ROOT / 'tools' / 'assets', ROOT / 'tools' / 'studio'):
    sys.path.insert(0, str(directory))
import serve


class TextUsesTests(unittest.TestCase):
    def test_procedures_by_string(self):
        procs = {5010: {'texts': [7542, 7542, 7543]}, 5011: {'texts': [7543]}, 5012: {'texts': []}}
        names = types.SimpleNamespace(procs={'5010': {'name': 'brothers_reply_none'}, '5011': {}})
        files = [{'kind': 'scene', 'entry': 300, 'procs': [5010]}, {'kind': 'shared', 'entry': None, 'procs': [5011]}]
        world = {'areas': [{'area': 0, 'place': 'Ancient Tree', 'detail': 'side room'}], 'scene_area': {300: 0},
                 'scenes': {300: {'label': 'Bat ambush'}}}
        import gclfiles
        import gclview
        with patch.dict(serve._TEXT_USES, {}, clear=True), patch.object(serve.actors, 'procs', lambda: procs), \
                patch.object(gclview, 'names', lambda: names), patch.object(gclfiles, 'files', lambda: files), \
                patch.object(serve, 'world', lambda: world):
            self.assertEqual(serve.text_uses(7542), {'procs': [{'id': 5010, 'name': 'brothers_reply_none', 'scene': 300,
                                                                'place': 'Ancient Tree · Bat ambush', 'area': 0}]})
            self.assertEqual([(p['id'], p['place']) for p in serve.text_uses(7543)['procs']], [(5010, 'Ancient Tree · Bat ambush'), (5011, None)])
            self.assertEqual(serve.text_uses(1), {'procs': []})


if __name__ == '__main__':
    unittest.main()
