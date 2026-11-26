"""Run with build/venv/bin/python -m unittest discover -s port/tests -p verify_test.py."""
from contextlib import redirect_stdout
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

import verify


class TestResume(unittest.TestCase):
    def test_bank_resume_requires_the_same_inputs_and_complete_result(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            save = root / 'build/saves/0001.sav'
            wasm = root / 'build/port/game/game.wasm'
            types = root / 'build/port/agent/types.json'
            for file in (save, wasm, types):
                file.parent.mkdir(parents=True, exist_ok=True)
                file.write_bytes(b'initial')

            bank = save.parent
            out = root / 'results'
            result = out / 'fuzz-1-2800/0001/result.json'
            calls = []
            difference = None

            def run_case(checkpoint, case_out, frames, seed, recording, rom_reference):
                calls.append((checkpoint, seed))
                case_out.mkdir(parents=True, exist_ok=True)
                (case_out / 'result.sha256').unlink(missing_ok=True)
                result.write_text(json.dumps({'ticks': 10, 'difference': difference, 'fault': None}))
                return int(bool(difference))

            with (patch.object(verify, 'ROOT', root),
                  patch.object(verify, 'run_case', side_effect=run_case),
                  patch.object(verify.shutil, 'which', return_value=sys.executable),
                  patch.object(sys, 'argv', ['verify.py', '--all', '--bank', str(bank),
                                            '--out-root', str(out), '--resume']),
                  redirect_stdout(io.StringIO())):
                result.parent.mkdir(parents=True)
                result.write_text('{"ticks":20,"difference":null,"fault":null}')
                self.assertEqual(verify.main(), 0)
                self.assertEqual(len(calls), 1, 'legacy output needs a replay')
                self.assertEqual(verify.main(), 0)
                self.assertEqual(len(calls), 1, 'an unchanged matching result is reusable')
                for count, file in enumerate((wasm, save, types, result), start=2):
                    file.write_bytes(b'changed')
                    self.assertEqual(verify.main(), 0)
                    self.assertEqual(len(calls), count, f'changing {file.name} forces a replay')

                result.unlink()
                difference = {'tick': 3, 'changes': {'hudPage': {}}}
                self.assertEqual(verify.main(), 1)
                self.assertFalse(result.with_name('result.sha256').exists())
                difference = None
                self.assertEqual(verify.main(), 0)
                self.assertEqual(len(calls), 7, 'a failed comparison is retried')
