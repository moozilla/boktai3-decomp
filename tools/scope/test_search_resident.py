"""Independent sessions must not serialize compiler diagnostic work."""
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
import search


class ResidentIsolationTests(unittest.TestCase):
    def test_session_mailboxes_do_not_alias(self):
        with tempfile.TemporaryDirectory() as tmp, patch.object(search, 'BASE', Path(tmp)):
            first = search.mailbox_directory('first')
            second = search.mailbox_directory('second')
            self.assertNotEqual(first, second)
            self.assertEqual(first, Path(tmp) / 'first' / 'resident')

    def test_explicit_database_uses_its_own_directory(self):
        with tempfile.TemporaryDirectory() as tmp:
            db = Path(tmp) / 'isolated' / 'session.sqlite3'
            self.assertEqual(search.mailbox_directory(str(db)), db.resolve().parent / 'resident')


if __name__ == '__main__':
    unittest.main()
