import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import replay_support as replay


class ReplaySupportTest(unittest.TestCase):
    def test_reviewed_routes_keep_a_lane_only_header_and_the_reviewed_schema(self):
        base = b"current_path,new_path\ninclude/old.h,include/old.h\n"
        lane = base + b"include/lane.h,include/lane.h\n"
        reviewed = (b"current_path,new_path,prior_paths\n"
                    b"include/old.h,include/engine/old.h,include/old.h\n"
                    b"include/new.h,include/engine/new.h,\n")
        merged, added = replay.with_lane_additions(base, lane, reviewed, ("current_path",))
        self.assertEqual(added, 1)
        self.assertEqual(merged, reviewed + b"include/lane.h,include/lane.h,\n")
        self.assertEqual(replay.with_lane_additions(base, merged, merged, ("current_path",)), (merged, 0))

    def test_conflicting_new_rows_report_the_key(self):
        empty = b"current_path,new_path\n"
        lane = empty + b"include/lane.h,include/lane.h\n"
        reviewed = empty + b"include/lane.h,include/engine/lane.h\n"
        with self.assertRaisesRegex(ValueError, "include/lane.h"):
            replay.with_lane_additions(empty, lane, reviewed, ("current_path",))

    def test_unscoped_legacy_tables_match_the_reviewed_global_rows(self):
        base = b"old_prefix,new_prefix\nOld_,New_\n"
        reviewed = b"old_prefix,new_prefix,scope\nOld_,Final_,all\n"
        result = replay.with_lane_additions(base, base, reviewed, ("old_prefix", "scope"))
        self.assertEqual(result, (reviewed, 0))

    def test_branch_edits_follow_a_planned_source_move(self):
        old = Path("src/monsters/enemy_particle/new.c")
        new = Path("src/effects/particles/enemy_attacks/new.c")
        unrelated = Path("src/engine/other.c")
        self.assertEqual(replay.protect_destinations({old}, {old: new, unrelated: unrelated}),
                         {old, new})

    def test_wrap_only_targets_lines_pushed_past_the_limit(self):
        before = "short\nvery long line here\nshort\n"
        after = "now too long\nlonger than the existing line\nstill fine\n"
        self.assertEqual(replay.reorg.overlong_rewrites(before, after, 10), [1])
