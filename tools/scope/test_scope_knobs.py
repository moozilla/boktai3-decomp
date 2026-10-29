"""Compiler selector regressions from a still-unmatched Bosses tail trial."""
import contextlib
import io
import os
import unittest
from unittest import mock

import scope


class NamedPseudoKnobTests(unittest.TestCase):
    def resolve(self, knobs, names=None):
        if names is None:
            names = {22: 'actor', 143: 'variant', 159: 'clamped'}
        with mock.patch.object(scope, 'compile_draft') as compile_draft, \
             mock.patch.object(scope, 'workdir', return_value='unused'), \
             mock.patch.object(scope, 'pseudo_names', return_value=names), \
             contextlib.redirect_stderr(io.StringIO()):
            with mock.patch.dict(os.environ, {}, clear=True):
                result = scope.resolve_knobs('candidate.c', 'Example', knobs)
            return result, compile_draft

    def test_named_cse_selector_resolves_before_compilation(self):
        result, compiled = self.resolve({'CSE_NOCANON': 'clamped'})
        self.assertEqual(result['CSE_NOCANON'], '159')
        compiled.assert_called_once_with('candidate.c', 'Example', {})

    def test_keep_list_resolves_names_and_preserves_numeric_pseudo(self):
        result, compiled = self.resolve({'CSE_KEEP': 'clamped,22,variant'})
        self.assertEqual(result['CSE_KEEP'], '159,22,143')
        compiled.assert_called_once()

    def test_numeric_selector_needs_no_name_resolution_compile(self):
        result, compiled = self.resolve({'CSE_NOCANON': '159', 'CSE_KEEP': '22,143'})
        self.assertEqual(result, {'CSE_NOCANON': '159', 'CSE_KEEP': '22,143'})
        compiled.assert_not_called()

    def test_missing_cse_name_is_not_passed_to_compiler_atoi(self):
        with self.assertRaisesRegex(SystemExit, 'no pseudo by that name'):
            self.resolve({'CSE_NOCANON': 'typo'})

    def test_ambiguous_cse_name_requires_explicit_pseudo(self):
        with self.assertRaisesRegex(SystemExit, 'names pseudos'):
            self.resolve({'CSE_KEEP': 'value'}, {31: 'value', 32: 'value'})

    def test_ambient_cse_names_resolve_and_restore_environment(self):
        ambient = {'CSE_KEEP': 'clamped', 'CSE_NOCANON': 'variant'}
        def baseline(*args):
            self.assertNotIn('CSE_KEEP', os.environ)
            self.assertNotIn('CSE_NOCANON', os.environ)
        with mock.patch.object(scope, 'compile_draft', side_effect=baseline) as compiled, \
             mock.patch.object(scope, 'workdir', return_value='unused'), \
             mock.patch.object(scope, 'pseudo_names', return_value={143: 'variant', 159: 'clamped'}), \
             mock.patch.dict(os.environ, ambient, clear=True), \
             contextlib.redirect_stderr(io.StringIO()):
            result = scope.resolve_knobs('candidate.c', 'Example', {})
            self.assertEqual(result, {'CSE_KEEP': '159', 'CSE_NOCANON': '143'})
            self.assertEqual(dict(os.environ), ambient)
            compiled.assert_called_once_with('candidate.c', 'Example', {})

    def test_missing_ambient_cse_name_still_restores_environment(self):
        with mock.patch.object(scope, 'compile_draft'), \
             mock.patch.object(scope, 'workdir', return_value='unused'), \
             mock.patch.object(scope, 'pseudo_names', return_value={}), \
             mock.patch.dict(os.environ, {'CSE_KEEP': 'missing'}, clear=True):
            with self.assertRaises(SystemExit):
                scope.resolve_knobs('candidate.c', 'Example', {})
            self.assertEqual(os.environ['CSE_KEEP'], 'missing')

    def test_named_cse_and_allocation_selectors_share_baseline(self):
        result, compiled = self.resolve({'CSE_KEEP': 'clamped', 'AGBCC_PRI': 'variant:+100',
                                         'AGBCC_FORCE': 'actor:7'})
        self.assertEqual(result, {'CSE_KEEP': '159', 'AGBCC_PRI': '143:+100', 'AGBCC_FORCE': '22:7'})
        compiled.assert_called_once_with('candidate.c', 'Example', {})


if __name__ == '__main__':
    unittest.main()
