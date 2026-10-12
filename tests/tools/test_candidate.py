import sys
from pathlib import Path
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
from candidate import classify


class CandidateTests(unittest.TestCase):
    def test_compile_error_never_becomes_mismatch_or_match(self):
        self.assertEqual(classify(1, None), 'error')
        self.assertEqual(classify(1, {'status': 'match'}), 'error')
        self.assertEqual(classify(0, {'status': 'mismatch'}), 'error')
        self.assertEqual(classify(2, {'status': 'mismatch'}), 'error')

    def test_consistent_results(self):
        self.assertEqual(classify(0, {'status': 'match'}), 'match')
        self.assertEqual(classify(1, {'status': 'mismatch'}), 'mismatch')
