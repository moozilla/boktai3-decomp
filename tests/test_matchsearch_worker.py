"""Worker conveniences: source fidelity, truthful annotations, and landing rollback.

Compiler verdicts are explicit stand-ins. CPP, three-way merge, formatter/check
child processes, source replacement, and rollback operate on real temporary files.
"""
from __future__ import annotations

import contextlib
import io
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tools.matchsearch import worker
from tools.matchsearch.__main__ import main
from tools.matchsearch.store import Store


BASE = '''// keep the unit's note
#define FRAME(x) ((x) + 0)
#define PIN_REG "r2"

#ifdef NONMATCHING
// fuzzy: 12.3%
int Target(int x)
{
    // keep the author's explanation
    register int pinned asm(PIN_REG) = x;
    return 2;
}
#else
INCLUDE_ASM("asm/nonmatching", Target);
#endif

// neighbor belongs to another worker
int Neighbor(void) { return FRAME(9); }
'''


@unittest.skipUnless(shutil.which('cc') and shutil.which('git'), 'CPP and git merge-file are required')
class LocalLanding(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.repo = Path(directory.name)
        self.target = self.repo / 'src' / 'unit.c'
        self.target.parent.mkdir()
        self.target.write_text(BASE)
        self.candidate = BASE.replace('return 2;', 'return 1;')
        scope = self.repo / 'tools' / 'scope'
        scope.mkdir(parents=True)
        shutil.copy(ROOT / 'tools' / 'scope' / 'land.py', scope / 'land.py')
        self.formatter = self.repo / 'build' / 'venv' / 'bin' / 'clang-format'
        self.formatter.parent.mkdir(parents=True)
        self.script(self.formatter, 'pass')
        self.checker = self.repo / 'tools' / 'check.py'
        self.checker.write_text('print("unit: MATCH")\n')
        self.store = Store(self.repo / 'session')
        self.addCleanup(self.store.close)
        self.store.bind_context({'test': True})
        root, _ = self.store.add(BASE, label='baseline')
        self.store.set_meta('root', root)
        self.node, _ = self.store.add(self.candidate, parent=root, label='matching')
        self.production = SimpleNamespace(exact=True, diagnostic_knobs={}, error=None)
        self.verified = []
        self.evaluator = SimpleNamespace(
            repo=self.repo, source_path=self.target, function='Target', own_names=('Target',),
            address=0x08000000, production_compiler=Path('/test/stock-agbcc'), cpp=[shutil.which('cc'), '-E'],
            validate_environment=lambda: None, evaluate=lambda source: self.production,
            preprocess=self.preprocess, verify=self.verify)
        self.session = SimpleNamespace(evaluator=self.evaluator, store=self.store)

    def script(self, path, body):
        path.write_text(f'#!{sys.executable}\n' + body + '\n')
        path.chmod(0o755)

    def preprocess(self, source):
        return subprocess.run([shutil.which('cc'), '-E', '-P', '-DNONMATCHING', '-x', 'c', '-'],
                              input=source, capture_output=True, text=True, check=True).stdout

    def verify(self, source):
        self.verified.append(source)
        return {'exact': True, 'code_hash': 'test-only-stock-result'}

    def test_symlinked_repository_root_preserves_src_containment(self):
        alias = self.repo / 'repository-link'
        alias.symlink_to(self.repo, target_is_directory=True)
        self.evaluator.repo = alias
        self.evaluator.source_path = alias / 'src' / 'unit.c'
        result = worker.land(self.session, self.node, dry_run=True)
        self.assertFalse(result['landed'])
        self.assertTrue(result['verification']['exact'])
        self.assertEqual(self.target.read_text(), BASE)

    def test_source_symlink_outside_src_remains_rejected(self):
        outside = self.repo / 'outside.c'
        outside.write_text(BASE)
        self.target.unlink()
        self.target.symlink_to(outside)
        with self.assertRaisesRegex(ValueError, 'existing src/ C unit'):
            worker.land(self.session, self.node)
        self.assertEqual(outside.read_text(), BASE)

    def test_merge_preserves_macros_comments_and_current_neighbor_changes(self):
        self.target.write_text(BASE.replace('// neighbor belongs to another worker', '// reviewed neighbor note'))
        result = worker.land(self.session, self.node)
        landed = self.target.read_text()
        self.assertTrue(result['landed'])
        self.assertEqual(result['concessions']['register_pins'], 1)
        self.assertIn('// MATCHING_CONCESSION: register-pinning', landed)
        self.assertIn('#define PIN_REG "r2"', landed)
        self.assertIn('asm(PIN_REG)', landed)
        self.assertIn('// keep the author\'s explanation', landed)
        self.assertIn('// reviewed neighbor note', landed)
        self.assertIn('int Neighbor(void) { return FRAME(9); }', landed)
        self.assertNotIn('INCLUDE_ASM', landed)
        self.assertNotIn('#ifdef NONMATCHING', landed)
        self.assertNotIn('fuzzy:', landed)
        self.assertEqual(self.verified[-1], landed)

    def test_checker_failure_restores_the_original_after_formatter_changed_it(self):
        self.script(self.formatter, 'import pathlib,sys\np=pathlib.Path(sys.argv[-1])\np.write_text(p.read_text()+"// formatted\\n")')
        self.checker.write_text('import sys\nprint("unit: MISMATCH")\nsys.exit(1)\n')
        before = self.target.read_bytes()
        with self.assertRaisesRegex(RuntimeError, 'unit_check failed'):
            worker.land(self.session, self.node)
        self.assertEqual(self.target.read_bytes(), before)
        self.assertTrue(list((self.store.directory / 'landing').glob('*/unit_check.log')))

    def test_formatter_launch_failure_and_timeout_restore_original_bytes(self):
        for mode in ('missing-interpreter', 'timeout'):
            with self.subTest(mode=mode):
                if mode == 'missing-interpreter':
                    self.formatter.write_text('#!/no/such/interpreter\n')
                    exception = OSError
                else:
                    self.script(self.formatter, 'import time\ntime.sleep(10)')
                    exception = subprocess.TimeoutExpired
                before = self.target.read_bytes()
                with self.assertRaises(exception):
                    worker.land(self.session, self.node, timeout=0.05)
                self.assertEqual(self.target.read_bytes(), before)

    def test_interruption_during_formatter_also_restores_source(self):
        run = subprocess.run
        alias = self.repo / 'repository-link'
        alias.symlink_to(self.repo, target_is_directory=True)

        for repo in (self.repo, alias):
            with self.subTest(root=repo):
                self.evaluator.repo = repo
                self.evaluator.source_path = repo / 'src' / 'unit.c'
                formatter = repo / 'build' / 'venv' / 'bin' / 'clang-format'

                def interrupted(command, **kwargs):
                    # land resolves the root, including macOS /var -> /private/var.
                    if Path(command[0]).resolve() == formatter.resolve():
                        self.assertNotEqual(self.target.read_text(), BASE)
                        raise KeyboardInterrupt()
                    return run(command, **kwargs)

                with patch.object(worker.subprocess, 'run', side_effect=interrupted):
                    with self.assertRaises(KeyboardInterrupt):
                        worker.land(self.session, self.node)
                self.assertEqual(self.target.read_text(), BASE)

    def test_failed_stock_gate_and_diagnostic_match_never_write_source(self):
        self.evaluator.verify = lambda _: {'exact': False, 'byte_mismatches': 2}
        with self.assertRaisesRegex(ValueError, 'stock-compiler'):
            worker.land(self.session, self.node)
        self.assertEqual(self.target.read_text(), BASE)
        self.production.diagnostic_knobs = {'AGBCC_FORCE': '1:2'}
        with self.assertRaisesRegex(ValueError, 'ordinary production'):
            worker.land(self.session, self.node)
        self.assertEqual(self.target.read_text(), BASE)

    def test_dry_run_is_concrete_verified_and_does_not_touch_source(self):
        result = worker.land(self.session, self.node, dry_run=True)
        self.assertFalse(result['landed'])
        self.assertTrue(result['verification']['exact'])
        self.assertIn('return 1;', (Path(result['evidence_directory']) / 'proposed.c').read_text())
        self.assertEqual(self.target.read_text(), BASE)

    def test_local_name_alias_unwraps_the_actual_guard_and_preserves_the_macro(self):
        aliased = '#define LocalTarget Target\n' + BASE.replace('Target', 'LocalTarget')
        self.target.write_text(aliased)
        root, _ = self.store.add(aliased)
        self.store.set_meta('root', root)
        node, _ = self.store.add(aliased.replace('return 2;', 'return 1;'), parent=root)
        worker.land(self.session, node)
        self.assertIn('#define LocalTarget Target', self.target.read_text())
        self.assertIn('int LocalTarget(int x)', self.target.read_text())
        self.assertNotIn('INCLUDE_ASM', self.target.read_text())

    def test_unit_with_no_active_target_definition_cannot_pass_on_its_neighbors(self):
        guarded = self.candidate.replace('#else\nINCLUDE_ASM("asm/nonmatching", Target);\n', '')
        node, _ = self.store.add(guarded, parent='root')
        with self.assertRaisesRegex(ValueError, 'not active in the normal build'):
            worker.land(self.session, node)
        self.assertEqual(self.target.read_text(), BASE)

    def test_unhandled_guard_and_instruction_asm_cannot_silently_land(self):
        for source, reason in (
                (self.candidate.replace('#ifdef NONMATCHING', '#if defined(NONMATCHING)'), 'fallback remains'),
                (self.candidate.replace('return 1;', 'asm("mov r0, r1"); return 1;'), 'instruction asm')):
            with self.subTest(reason=reason):
                node, _ = self.store.add(source, parent='root')
                if reason == 'fallback remains':
                    # Both current and baseline use the same unrecognized form,
                    # so this is a wrapper-validation failure, not a merge clash.
                    self.target.write_text(BASE.replace('#ifdef NONMATCHING', '#if defined(NONMATCHING)'))
                    with patch.object(self.store, 'source', side_effect=lambda n: (
                            BASE.replace('#ifdef NONMATCHING', '#if defined(NONMATCHING)') if n == 'root' else source)):
                        with self.assertRaisesRegex(ValueError, reason):
                            worker.land(self.session, node)
                    self.target.write_text(BASE)
                else:
                    with self.assertRaisesRegex(ValueError, reason):
                        worker.land(self.session, node)
                self.assertEqual(self.target.read_text(), BASE)


class TextInterface(unittest.TestCase):
    def test_propose_reads_file_verbatim_without_manual_json_escaping(self):
        with tempfile.TemporaryDirectory() as td:
            source = Path(td) / 'draft.c'
            source.write_text(BASE)
            reply = {'ok': True, 'result': {'id': 'candidate', 'evaluation': {
                'status': 'compiled', 'candidate_size': 20, 'target_size': 22,
                'instruction_diff': 1, 'concessions': {'register_pins': 1}}}}
            out = io.StringIO()
            with patch('tools.matchsearch.__main__.request', return_value=reply) as request, contextlib.redirect_stdout(out):
                self.assertEqual(main(['propose', td, str(source), '--hypothesis', 'test the helper boundary']), 0)
            self.assertEqual(request.call_args.args[1]['args']['source'], BASE)
            self.assertIn('Node candidate', out.getvalue())
            self.assertNotIn('"evaluation"', out.getvalue())

    def test_debug_changes_cannot_be_presented_as_production_line_annotations(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            rows = [(0, 2, 'movs r0, #1', 'movs r0, #1', None)]
            scope = SimpleNamespace(
                compile_draft_g=lambda *args: (b'debug', 0, {'wd': td, 'data': [], 'obj_name': 'Target', 'obj_offset': 0}),
                decode=lambda *args: rows,
                draft_annotations=lambda *args: self.fail('must not label a different production stream'))
            evaluator = SimpleNamespace(scope=scope, _input=root / 'candidate.c', workdir=root,
                                        function='Target', address=0x08000000,
                                        _scope_context=lambda **kwargs: contextlib.nullcontext())
            annotation, note = worker._annotations(evaluator, 'int Target(void) { return 1; }', b'production', rows)
            self.assertEqual(annotation, {})
            self.assertTrue(note['debug_changes_code'])
            self.assertIn('withheld', note['authority'])


if __name__ == '__main__':
    unittest.main()
