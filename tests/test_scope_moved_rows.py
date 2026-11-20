"""The order-difference summary only suggests actionable RTL diagnostics."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class MovedRowsNote(unittest.TestCase):
    def setUp(self):
        self.scope, _ = _load_scope(Path(__file__).resolve().parents[1])
        self.rows = [
            {"tag": "+", "draft": ".word 0xFFFF0000", "uid": None},
            {"tag": "-", "rom": ".word 0xFFFF0000"},
        ]

    def test_moved_data_has_no_why_command(self):
        self.assertEqual(self.scope.moved_rows_note(self.rows),
                         "Moved instruction pairs (1)")

    def test_uses_the_first_annotated_move(self):
        rows = self.rows + [
            {"tag": "-", "rom": "movs r0, #0"},
            {"tag": "+", "draft": "movs r0, #0", "uid": 123},
        ]
        self.assertIn("`why 123`", self.scope.moved_rows_note(rows))
