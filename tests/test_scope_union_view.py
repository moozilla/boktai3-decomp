"""`hoist` writes the one-member union lever for a hoisted variable (one-block loop bodies)."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class UnionView(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_declaration_keeps_its_name(self):
        rows = ["    for (i = 0; i < 3; i++) {", "        const u16 *table = gRandomTable;", "        x = table[1];", "    }"]
        out = self.scope.union_view_variant(rows, 2, "table")
        self.assertEqual(out[1], "        union { const u16 * v; } tableView = {gRandomTable};")
        self.assertEqual(out[2], "        const u16 * table = tableView.v;")
        self.assertEqual(out[3:], rows[2:])

    def test_statement_finds_the_declared_type(self):
        rows = ["    u32 *cursor;", "    do {", "        cursor = &gRandomIndex;", "    } while (n--);"]
        out = self.scope.union_view_variant(rows, 3, "cursor")
        self.assertEqual(out[2], "        { union { u32 * v; } view = {&gRandomIndex}; cursor = view.v; }")

    def test_unknown_type_gives_nothing(self):
        rows = ["    s32 a, cursor;", "    for (;;) {", "        cursor = 3;", "    }"]
        self.assertIsNone(self.scope.union_view_variant(rows, 3, "cursor"))


if __name__ == "__main__":
    unittest.main()
