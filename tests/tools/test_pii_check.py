"""pii_check flags home paths and allows the neutral forms."""
import re
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
import pii_check


class PiiCheckTests(unittest.TestCase):
    def hits(self, text, extra=()):
        return pii_check.scan_line(text, pii_check.GENERIC + list(extra))

    def test_flags_home_paths(self):
        for text in ('/Users/alice/git/x', 'cd /home/alice/', r'C:\Users\alice'):
            self.assertTrue(self.hits(text), text)

    def test_allows_neutral_forms(self):
        for text in ('~/git/boktai3', '/home/user/boktai3-decomp', 'chase/home/terrain', 'a@example.com'):
            self.assertEqual(self.hits(text), [], text)

    def test_private_patterns_match_without_echoing(self):
        hits = self.hits('by Secret Name', [('private pattern', re.compile('secret name', re.I))])
        self.assertEqual(hits, ['private pattern'])


if __name__ == '__main__':
    unittest.main()
