import sys
from pathlib import Path
from types import SimpleNamespace
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
import check


class ComparisonTests(unittest.TestCase):
    def test_verdict_only_keeps_reviewed_extent(self):
        args = SimpleNamespace(nonmatching=False, target_size=4, match_only=True)
        rom = b'\x01\x02\x03\x04'
        for candidate, exit_code in ((rom, 0), (rom[:2], 1), (rom[::-1], 1)):
            with self.subTest(candidate=candidate):
                linked = ('test.o', 0x08000000, ['Test'], candidate)
                with (patch.object(check, 'linked_bytes', return_value=linked),
                      patch.object(check, 'load_rom', return_value=rom),
                      patch.object(check, 'comparison') as detailed,
                      patch.object(check, 'print_verdict') as verdict,
                      self.assertRaises(SystemExit) as result):
                    check.run(args, 'test.c', 'build/test')
                self.assertEqual(result.exception.code, exit_code)
                verdict.assert_called_once_with(['Test'], 0x08000000, len(candidate), exit_code == 0)
                detailed.assert_not_called()

    def test_short_prefix_is_not_full_match(self):
        r = check.comparison(b'\x00\x00\x01\x00', b'\x00\x00', 0x08000000, target_size=4)
        self.assertEqual(r['status'], 'mismatch')
        self.assertEqual(r['size_delta'], -2)
        self.assertEqual(r['different_byte_count'], 2)

    def test_legacy_extent_is_explicit(self):
        r = check.comparison(b'\x00\x00', b'\x00\x00', 0)
        self.assertEqual(r['status'], 'match')
        self.assertEqual(r['extent_basis'], 'candidate-length')
        self.assertIsNone(r['size_delta'])

    def test_insertion_does_not_spoil_rest_of_alignment(self):
        r = dict(original=[(i*2, f'op{i}') for i in range(30)],
                 candidate=[(0, 'extra')] + [(i*2+2, f'op{i}') for i in range(30)],
                 different_byte_count=2)
        compact = check.render_diff(r, context=2)
        self.assertIn('extra', compact)
        self.assertNotIn('op29', compact)
        self.assertLess(len(compact.splitlines()), 8)
        self.assertIn('op29', check.render_diff(r, full=True))

    def test_changed_branch_target_is_visible(self):
        r = dict(original=[(0, 'b #0x20')], candidate=[(0, 'b #0x22')], different_byte_count=1)
        self.assertIn('b #0x20', check.render_diff(r))
        self.assertIn('b #0x22', check.render_diff(r))

    def test_identical_decode_does_not_hide_byte_failure(self):
        with patch.object(check, 'disasm', return_value=[(0, 'same')]):
            r = check.comparison(b'\x01', b'\x02', 0, target_size=1)
        self.assertEqual(r['status'], 'mismatch')
        self.assertIn('resolved bytes differ', check.render_diff(r))
