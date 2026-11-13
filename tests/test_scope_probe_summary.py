"""A probe without improvement must not claim that compiler passes did not change code."""
import contextlib
import io
from pathlib import Path
import unittest
from tools.matchsearch.evaluator import _load_scope

class ProbeSummary(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def report(self, tried, regressions):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            self.scope.probe_no_improvement(tried, regressions)
        return out.getvalue()

    def test_worsening_placement_probe_is_visible(self):
        out = self.report(13, [("AGBCC_NOPRE", "5", "9 ROM / 9 draft")])
        self.assertIn("improves the count", out)
        self.assertIn("AGBCC_NOPRE=5: 9 ROM / 9 draft", out)
        self.assertIn("may still interact", out)
        self.assertNotIn("the difference is elsewhere", out)
        self.assertNotIn("None of these probes changed", out)

    def test_equal_counts_do_not_rule_out_interactions(self):
        out = self.report(7, [])
        self.assertIn("None of these probes changed the count", out)
        self.assertIn("interacting decisions are still possible", out)
        self.assertNotIn("the difference is elsewhere", out)

    def test_worsening_list_has_a_bound_and_a_remainder(self):
        out = self.report(8, [("AGBCC_NOPRE", str(i), "worse") for i in range(8)])
        self.assertEqual(out.count("AGBCC_NOPRE="), 4)
        self.assertIn("4 more worsening probes", out)

if __name__ == "__main__":
    unittest.main()
