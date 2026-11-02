"""An instruction budget limits what the semantic oracle can conclude."""
from pathlib import Path
import sys
from types import SimpleNamespace
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import semcheck


def execution(status, callees):
    calls = [(name, [0] * 4, [], ({}, {})) for name in callees]
    return SimpleNamespace(status=status, calls=calls, reads={}, writes={}, writer={}, r0=0)


def check(rom, draft):
    oracle = semcheck.Oracle.__new__(semcheck.Oracle)
    oracle.start, oracle.rom, oracle.names, oracle.arity = 0, b"", {}, {}
    oracle.runs, oracle.check_r0 = [((1000, []), rom)], True
    with patch.object(semcheck, "Run", return_value=draft):
        return oracle.check(b"")


class InstructionLimits(unittest.TestCase):
    def test_prefixes_at_different_iterations_are_incomplete(self):
        short = execution("step limit", ["update"])
        long = execution("step limit", ["update", "update"])
        long.writes[12] = 3
        for a, b in ((short, long), (long, short)):
            self.assertEqual(check(a, b)[0], "INCOMPLETE")

    def test_step_limit_on_only_one_side(self):
        capped = execution("step limit", ["update"])
        finished = execution("ok", ["update", "finish"])
        for a, b in ((capped, finished), (finished, capped)):
            self.assertEqual(check(a, b)[0], "INCOMPLETE")
        capped.calls.append(finished.calls[-1])
        finished.reads[4] = 12
        self.assertEqual(check(capped, finished)[0], "INCOMPLETE")

    def test_observed_address_absent_from_finished_run(self):
        for field in ("reads", "writes"):
            capped = execution("step limit", [])
            finished = execution("ok", ["finish"])
            getattr(capped, field)[12] = 3
            capped.writer[12] = 16
            for a, b in ((capped, finished), (finished, capped)):
                self.assertEqual(check(a, b)[0], "DIFFERENT")

    def test_extra_call_after_other_run_returned_is_real(self):
        longer = execution("step limit", ["update", "extra"])
        finished = execution("ok", ["update"])
        for a, b in ((longer, finished), (finished, longer)):
            self.assertEqual(check(a, b)[0], "DIFFERENT")

    def test_capped_runs_can_disagree_before_the_limit(self):
        rom = execution("step limit", ["destroy"])
        draft = execution("step limit", ["update"])
        self.assertEqual(check(rom, draft)[0], "DIFFERENT")
