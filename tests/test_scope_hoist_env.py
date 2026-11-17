"""Hoist tracing must accept an explicit trace knob and retain other overrides."""
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from tools.matchsearch.evaluator import _load_scope


class HoistEnvironment(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_trace_is_required_without_overriding_other_knobs(self):
        trace = (
            "loopopt fn=Update\n"
            "cand fn=Update loop=10 insn=20 regno=30 may_not=0 maybe_never=0 "
            "used_before=0 in_block=1 set_in_loop=1 invariant=1\n"
            "move fn=Update insn=20 regno=30 threshold=26 savings=1 lifetime=3 "
            "insn_count=20 moved_once=0 already_moved=0\n"
        )
        ambient = {"AGBCC_LOOP_TRACE": "0", "AGBCC_NOMOVE": "99", "HOST_ONLY": "kept"}
        for requested_trace in (None, "0", "1"):
            with self.subTest(trace=requested_trace):
                knobs = {"AGBCC_NOMOVE": "20"}
                if requested_trace is not None:
                    knobs["AGBCC_LOOP_TRACE"] = requested_trace
                original = knobs.copy()
                info = {"wd": "scratch", "obj_name": "Update", "env": knobs}
                output = StringIO()
                with patch.dict(self.scope.os.environ, ambient, clear=True), \
                     patch.object(self.scope, "score", return_value=(0, info)) as score, \
                     patch.object(self.scope.build, "cflags_for", return_value=[]), \
                     patch.object(self.scope, "rom_name", return_value="Update"), \
                     patch.object(self.scope, "pseudo_names", return_value={30: "mask"}), \
                     patch.object(self.scope, "uid_lines", return_value={20: 4}), \
                     patch.object(self.scope.subprocess, "run",
                                  return_value=SimpleNamespace(stderr=trace)) as run, \
                     redirect_stdout(output):
                    self.assertEqual(self.scope.cmd_hoist("draft.c", "Update", knobs), 0)
                    self.assertEqual(dict(self.scope.os.environ), ambient)
                score.assert_called_once_with("draft.c", "Update", original, full=True)
                run.assert_called_once()
                self.assertEqual(run.call_args.kwargs["env"], {
                    "AGBCC_LOOP_TRACE": "1", "AGBCC_NOMOVE": "20", "HOST_ONLY": "kept",
                })
                self.assertEqual(knobs, original)
                self.assertIn("loop invariants, 1 loop pass(es)", output.getvalue())
                self.assertIn("26 x 1 x 3 >= 20", output.getvalue())


if __name__ == "__main__":
    unittest.main()
