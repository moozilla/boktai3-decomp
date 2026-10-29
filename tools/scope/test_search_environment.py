"""Integration seams that previously lost evidence or selected a stale HEAD."""
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock
import subprocess
import types

sys.path.insert(0, str(Path(__file__).resolve().parent))
import search
from search_session import Session


class FakeEnvironment:
    version = 1
    calls = 0

    def context(self, source, function, origin):
        return {"version": self.version, "function": function, "origin": origin}

    def evaluate(self, source, function, origin, knobs, trials):
        self.calls += 1
        return {"status": "different", "exact": False, "counts": "1 ROM / 1 draft",
                "experiment": bool(knobs), "semantic": {"verdict": "NOT_RUN"}}


class SearchIntegrationTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.path = Path(self.tmp.name)
        self.db = str(self.path / "test.sqlite3")
        self.source = self.path / "draft.c"
        self.source.write_text("int F(int x) { return x+1; }\n")
        self.env = FakeEnvironment()
        self.patch = mock.patch.object(search, "environment", return_value=self.env)
        self.patch.start()
        self.command("init", source=str(self.source), function="F")

    def tearDown(self):
        self.patch.stop()
        self.tmp.cleanup()

    def command(self, command, **kwargs):
        return search.dispatch({"command": command, "session": self.db, **kwargs})

    def test_import_uses_current_context_and_head_is_evaluated(self):
        self.env.version = 2
        self.source.write_text("int F(int x) { return 1+x; }\n")
        result = self.command("import", source=str(self.source), reason="commute")
        status = self.command("status")
        self.assertEqual(result["node"], status["node"])
        self.assertEqual(status["status"], "different")
        self.assertEqual(status["nodes"], 2)  # no unevaluated intermediate node
        with Session(self.db) as session:
            self.assertEqual(session.context()["version"], 2)

    def test_environment_refresh_selects_refreshed_head(self):
        old = self.command("status")["node"]
        self.env.version = 2
        result = self.command("evaluate")
        self.assertNotEqual(old, result["node"])
        self.assertEqual(result["node"], self.command("status")["node"])

    def test_cache_hit_and_forced_evaluation_both_work_with_leases(self):
        self.assertTrue(self.command("evaluate")["cache_hit"])
        self.assertEqual(self.env.calls, 1)
        self.assertFalse(self.command("evaluate", force=True)["cache_hit"])
        self.assertEqual(self.env.calls, 2)

    def test_experiments_do_not_replace_plain_evaluation(self):
        self.command("experiment", knobs={"AGBCC_FORCE": "16:0"})
        with Session(self.db) as session:
            plain = session.evaluation(options={"knobs": {}, "semantic_trials": 0})
            self.assertFalse(plain["result"]["experiment"])
            self.assertEqual(len(session.node()["evaluations"]), 2)

    def test_register_labels_are_accepted_but_instruction_bodies_rejected(self):
        self.source.write_text('int F(int x) { register int a asm("r1") = x; return a; }')
        self.command("import", source=str(self.source))
        self.source.write_text('int F(int x) { asm("mov r0, r1"); return x; }')
        with self.assertRaisesRegex(ValueError, "instruction bodies"):
            self.command("import", source=str(self.source))

    def test_explicit_trace_reference_reports_refreshed_node(self):
        self.env.scope = types.SimpleNamespace(WORK="")
        self.env.materialize = lambda source, origin: self.source
        old = self.command("status")["node"]
        self.env.version = 2
        with mock.patch("search_trace.collect", return_value={"allocations": [], "reloads": [],
                                                              "trace_matches_build": True}):
            result = self.command("trace", node=old)
        self.assertNotEqual(result["node"], old)
        self.assertEqual(result["node"], self.command("status")["node"])


class NativeIsolationTests(unittest.TestCase):
    def environment(self):
        from search_eval import Evaluator
        evaluator = object.__new__(Evaluator)
        evaluator.scope = types.SimpleNamespace(preprocess=lambda *a, **k: "", rom_name=lambda *a: "F")
        return evaluator

    def test_native_crash_cannot_escape_and_is_not_repeated(self):
        evaluator = self.environment()
        child = subprocess.CompletedProcess([], -4, "", "native fault")
        with mock.patch("subprocess.run", return_value=child) as run:
            for _ in range(2):
                result = evaluator.semantic("void F(void){}", "F", b"", b"", 0, "draft.c", 4)
                self.assertEqual(result["verdict"], "UNAVAILABLE")
                self.assertIn("signal 4", result["detail"])
            self.assertEqual(run.call_count, 1)

    def test_wall_clock_timeout_is_not_equivalence(self):
        evaluator = self.environment()
        with mock.patch("subprocess.run", side_effect=subprocess.TimeoutExpired("worker", 10)):
            result = evaluator.semantic("void F(void){}", "F", b"", b"", 0, "draft.c", 4)
        self.assertEqual(result["verdict"], "TIMEOUT")
        self.assertFalse(result["proof"])

    def test_request_releases_scope_dump_locks(self):
        import scope
        with tempfile.TemporaryFile() as stream:
            fd = os.dup(stream.fileno())
            def operation(request):
                scope._LOCKS["synthetic"] = fd
                return "ok"
            with mock.patch.object(search, "dispatch", side_effect=operation):
                self.assertTrue(search.safe_dispatch({"command": "probe"})["ok"])
            self.assertNotIn("synthetic", scope._LOCKS)
            with self.assertRaises(OSError):
                os.fstat(fd)


@unittest.skipUnless(os.environ.get("SEARCH_ROM_TESTS") == "1", "opt-in real unmatched compiler trial")
class RealUnmatchedEnvironmentTests(unittest.TestCase):
    def test_ambient_knobs_cannot_change_exact_environment(self):
        from search_eval import Evaluator
        root = Path(__file__).resolve().parents[2]
        origin = "src/fn/sub_08110C9C.c"
        source = (root / origin).read_text()
        if "NONMATCHING" not in source:
            self.skipTest("trial function has since matched; select another unmatched draft")
        with tempfile.TemporaryDirectory(dir=root / "build") as directory:
            evaluator = Evaluator(directory)
            try:
                plain = evaluator.evaluate(source, "sub_08110C9C", origin)
                with mock.patch.dict(os.environ, {"AGBCC_FORCE": "83:1"}):
                    ambient = evaluator.evaluate(source, "sub_08110C9C", origin)
                    self.assertEqual(plain["code_hash"], ambient["code_hash"])
                experiment = evaluator.evaluate(source, "sub_08110C9C", origin, {"AGBCC_FORCE": "83:1"})
                self.assertFalse(experiment["exact"])
                self.assertTrue(experiment["experiment"])
                self.assertNotEqual(plain["code_hash"], experiment["code_hash"])
                with mock.patch.object(evaluator, "semantic", side_effect=SystemExit("missing emulator")):
                    tested = evaluator.evaluate(source, "sub_08110C9C", origin, semantic_trials=2)
                self.assertEqual(tested["status"], "different")
                self.assertEqual(tested["semantic"]["verdict"], "UNAVAILABLE")
            finally:
                evaluator.close()


if __name__ == "__main__":
    unittest.main()
