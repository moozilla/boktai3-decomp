"""Run with build/venv/bin/python -m unittest discover -s port/tests -p gametypes_test.py."""
import json
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

from port.agent import gametypes


class TestGameTypes(unittest.TestCase):
    def test_nested_headers_keep_includes_owners_and_output_paths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            game = root / 'build/port/game'
            out = root / 'build/port/agent'
            for name, text in {
                'engine/shared.h': 'extern int gEngine;\nextern int gScene;\n',
                'scene/shared.h': 'extern int gScene;\n',
            }.items():
                header = game / 'include' / name
                header.parent.mkdir(parents=True)
                header.write_text(text)
            inventory = root / 'symbols/ram.csv'
            inventory.parent.mkdir()
            inventory.write_text('name,header\ngEngine,include/old/shared.h\n'
                                 'gScene,include/scene/shared.h\n')
            symbols = {'stamp': 'test', 'data': {'gEngine': 0x02000000, 'gScene': 0x02000004},
                       'code': {}}

            def compile_header(args, **kwargs):
                return subprocess.CompletedProcess(args, 0, '', '')

            with (patch.object(gametypes, 'ROOT', root),
                  patch.object(gametypes, 'GAME', game),
                  patch.object(gametypes, 'OUT', out),
                  patch.object(gametypes, 'symbols', return_value=symbols),
                  patch.object(gametypes, 'dwarf_entries', return_value={}),
                  patch.object(gametypes.subprocess, 'run', side_effect=compile_header) as compiler):
                output = gametypes.build(jobs=2)
                self.assertEqual(json.loads(output.read_text())['failed'], [])
                for header, variable in (('engine', 'gEngine'), ('scene', 'gScene')):
                    source = (out / 'types' / header / 'shared.c').read_text()
                    self.assertIn(f'#include "{header}/shared.h"', source)
                    self.assertIn(f' *__agent_var_{variable};', source)
                    self.assertEqual(source.count('__agent_var_'), 1)
                self.assertEqual(gametypes.build(jobs=2), output)
                self.assertEqual(compiler.call_count, 2)
