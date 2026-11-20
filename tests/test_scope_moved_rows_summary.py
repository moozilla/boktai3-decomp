"""Order diagnostics only suggest a why UID when a moved instruction has one."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class MovedRowsSummary(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_unannotated_move_uses_disassembly(self):
        rows = [
            {"tag": "-", "rom": ".word 0x1234"},
            {"tag": "+", "draft": ".word 0x1234", "uid": None},
        ]
        summary = self.scope.moved_rows_note(rows)
        self.assertIn("1 instruction at different positions", summary)
        self.assertIn("no RTL UIDs", summary)
        self.assertNotIn("why None", summary)

    def test_suggests_annotated_move_after_unannotated_move(self):
        rows = [
            {"tag": "-", "rom": ".word 0x1234"},
            {"tag": "+", "draft": ".word 0x1234", "uid": None},
            {"tag": "-", "rom": "movs r0, #3"},
            {"tag": "+", "draft": "movs r0, #3", "uid": 27},
        ]
        summary = self.scope.moved_rows_note(rows)
        self.assertIn("2 instructions at different positions", summary)
        self.assertIn("why 27", summary)

    def test_no_move_has_no_hint(self):
        self.assertEqual(self.scope.moved_rows_note([]), "")


if __name__ == "__main__":
    unittest.main()
