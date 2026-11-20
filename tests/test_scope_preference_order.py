import contextlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest import mock

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
                "K F 23 5 4 100 8 133 1\n"
                "K F 23 4 2 32 8 133 1\n"
                "E F 23 454 4\n"
                "F F 23 0 1 1ffff 1ffff 0 0 0 -1 1 4\n"
            )
            recs = scope.alloc_records(wd, "F")
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                scope.explain_find_reg(wd, "F", 23, 2, 4, str, recs, recs[23])
            self.assertIn("took r4 from local allocation", out.getvalue())
            self.assertIn("8 refs / 133 live insns against 2 refs / 32 live insns", out.getvalue())
            self.assertIn("evicted: 454.", out.getvalue())
            self.assertNotIn((23, 5), scope.local_takeovers(wd, "F"))
            self.assertNotIn("preference moved", out.getvalue())
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                scope.explain_alloc(wd, "F", 454, 4, 5, str, str)
            self.assertIn("took that register for 23", out.getvalue())

    def test_local_conflict_reports_the_holder_before_global_takeover(self):
        scope, _ = _load_scope(Path(__file__).resolve().parents[1])
        with tempfile.TemporaryDirectory() as wd:
            Path(wd, "names.txt").write_text(
                "L F 17 259 5 1134 26 264 9 2 6 spread\n"
                "L F 17 234 8 1165 38 244 8 0 5 table\n"
                "G F 263 1284 7 109 6 5 origin\n"
                "K F 263 5 8 228 7 109 1\n"
                "E F 263 234 5\n"
            )
            out = io.StringIO()
            insns = [(100, "insn", "(set (reg:SI 259) (reg:SI 263))")]
            with mock.patch.object(scope, "insn_list", return_value=insns), \
                    mock.patch.object(scope, "hard_of", return_value={234: 10, 259: 6, 263: 5}), \
                    mock.patch.object(scope, "conflicts", return_value={259: ({263}, {5})}), \
                    contextlib.redirect_stdout(out):
                scope.explain_alloc(wd, "F", 259, 5, 6, str, str)
            self.assertIn("during local allocation, r5 held 234", out.getvalue())
            self.assertIn("Global allocation later evicted it for 263", out.getvalue())
            self.assertIn("AGBCC_FORCE=table:10", out.getvalue())
