"""`why` notices when one PRE insertion serves replacements on two paths."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


def rec(k, expr, redundant, inserted):
    return {"k": k, "expr": expr, "redundant": redundant, "inserted": inserted, "copies": []}


class PreShared(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_two_paths_one_insertion(self):
        recs = [rec(1, 5, 621, [780]), rec(2, 5, 672, [780]), rec(3, 6, 700, [790])]
        self.assertEqual([r["k"] for r in self.scope.pre_shared_insertion(recs, recs[:1])], [1, 2])

    def test_single_replacement(self):
        recs = [rec(1, 5, 621, [780]), rec(3, 6, 700, [790])]
        self.assertEqual(self.scope.pre_shared_insertion(recs, recs[:1]), [])

    def test_copy_without_insertion(self):
        recs = [rec(1, 5, 621, []), rec(2, 5, 672, [])]
        self.assertEqual(self.scope.pre_shared_insertion(recs, recs), [])


if __name__ == "__main__":
    unittest.main()
