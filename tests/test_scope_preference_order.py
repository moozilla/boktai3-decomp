import contextlib
import io
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope


class PreferenceOrder(unittest.TestCase):
    def test_local_placement_keeps_its_rank_in_global_preference_pruning(self):
        scope, _ = _load_scope(Path(__file__).resolve().parents[1])
        with tempfile.TemporaryDirectory() as wd:
            Path(wd, "names.txt").write_text(
                "L F 3 25 0 10000 3 8 6 0 1 local\n"
                "G F 24 14000 12 20 0 4 parent\n"
                "G F 25 11000 6 12 0 1 local\n"
                "F F 24 0 0 07 1f 08 0 0 4 0 4\n"
            )
            Path(wd, "u.i.greg").write_text(";; 24 conflicts: 25\n;; 25 preferences: 3\n")
            records = scope.alloc_records(wd, "F")
            self.assertEqual(records[25][0], "L")
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                scope.explain_find_reg(wd, "F", 24, 3, 4, str, records, records[24])
            self.assertIn("25 prefers r3: global rank 2", out.getvalue())
            self.assertIn("local allocation already placed it in r1", out.getvalue())

    def test_global_allocation_can_evict_locals_without_a_register_preference(self):
        scope, _ = _load_scope(Path(__file__).resolve().parents[1])
        with tempfile.TemporaryDirectory() as wd:
            Path(wd, "names.txt").write_text(
                "L F 28 454 8 769 28 54 2 0 4 zero\n"
                "G F 23 1804 8 133 5 4 source\n"
                "G F 454 625 2 32 0 5 zero\n"
                "F F 23 0 0 1ffff 1ffff 0 0 0 -1 1 -1\n"
                "F F 23 0 1 1ffff 1ffff 0 0 0 -1 1 4\n"
            )
            recs = scope.alloc_records(wd, "F")
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                scope.explain_find_reg(wd, "F", 23, 2, 4, str, recs, recs[23])
            self.assertIn("took r4 from local allocation", out.getvalue())
            self.assertNotIn("preference moved", out.getvalue())
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                scope.explain_alloc(wd, "F", 454, 4, 5, str, str)
            self.assertIn("took that register for 23", out.getvalue())
