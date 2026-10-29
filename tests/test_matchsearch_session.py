"""Protocol/graph guarantees with a tiny deterministic compiler stand-in.

Actual agbcc, relocation, and source-export checks live in evaluator/move tests
and the recorded real-function experiments. These tests isolate orchestration.
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import re
import sys

import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.matchsearch.evaluator import Evaluation
from tools.matchsearch.session import Session
from tools.matchsearch.store import Store


class TinyCompiler:
    def __init__(self, repo, function, source_path, workdir=None):
        self.repo = Path(repo)
        self.function = function
        self.address = 0x08000000
        self.identity = {"compiler": "test-only", "function": function}
        self.target_code = b"\x01\x20\x70\x47"
        self.target_rows = self.rows(1)
        self.calls = 0
        self.rom_symbols = {}

    def preprocess(self, source):
        return source

    @staticmethod
    def rows(value):
        first = f"movs r0, #{value}"
        return [(0, 2, first, first, None), (2, 2, "bx lr", "bx lr", None)]

    def validate_environment(self):
        pass

    def validate_source_environment(self, source):
        pass

    def close(self):
        pass

    def evaluate(self, source, *, diagnostic_knobs=None, semcheck_trials=0):
        self.calls += 1
        value = int(re.search(r"return\s+(\d+)\s*;", source)[1])
        if diagnostic_knobs:
            value = 1
        code = bytes([value, 0x20, 0x70, 0x47])
        same = code == self.target_code
        result = Evaluation("exact" if same and not diagnostic_knobs else "compiled",
            same and not diagnostic_knobs, same and bool(diagnostic_knobs), self.function,
            self.address, hashlib.sha256(source.encode()).hexdigest(), "test-only", 4,
            candidate_size=4, byte_mismatches=int(not same), instruction_diff=int(not same),
            code_hash=hashlib.sha256(code).hexdigest(), code=code, rows=self.rows(value),
            diagnostic_knobs=diagnostic_knobs or {}, concessions={"register_pins": 0})
        if semcheck_trials:
            result.semantic = {"verdict": "counterexample", "proof": False, "trials": semcheck_trials}
        return result

    def verify(self, source):
        return {"exact": "return 1;" in source}


class SessionProtocolTests(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.tmp_path = Path(directory.name)

    def patch(self, target, value):
        replacement = patch(target, value)
        self.addCleanup(replacement.stop)
        return replacement.start()

    def test_session_roundtrip_diagnostic_isolation_and_sampled_counterexample(self):
        tmp_path = self.tmp_path
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        source = tmp_path / "source.c"
        source.write_text("int f(void) { return 2; }\n")
        directory = tmp_path / "session"
        with Session.create(directory, repo=tmp_path, function="f", source_path=source) as s:
            assert not s.store.node("root")["evaluation"]["exact"]
            counterfactual = s.diagnose("root", knobs={"AGBCC_FORCE": "22:0"})
            assert counterfactual["evidence"]["diagnostic_match"]
            assert not s.store.node("root")["evaluation"]["exact"]
            trial = s.semantic("root", trials=3)
            assert trial["semantic"]["verdict"] == "counterexample"
            assert not s.store.node("root")["rejected"]
            proposal = s.propose("int f(void) { return 1; }\n", hypothesis="correct the observable return")
            node = proposal["id"]
            assert proposal["evaluation"]["exact"]
            assert proposal["evaluation"]["region_summary"] == {"byte_exact": 1}
            attempts = s.evaluator.calls
            assert s.evaluate(node)["cached"]
            assert s.evaluator.calls == attempts
            out = tmp_path / "result.c"
            s.export(node, out)
            assert out.read_text() == s.store.source(node)
        with Session(directory) as s:
            assert s.status()["stats"]["exact_pin_free"] == 1
            assert s.store.node(node)["parents"][0]["hypothesis"] == "correct the observable return"
            assert s.verify(node)["verification"]["exact"]

    def test_initialization_can_resume_after_missing_toolchain(self):
        tmp_path = self.tmp_path
        source = tmp_path / "source.c"
        source.write_text("int f(void) { return 2; }\n")
        def missing(*args, **kwargs):
            raise FileNotFoundError("compiler")
        self.patch("tools.matchsearch.evaluator.Evaluator", missing)
        directory = tmp_path / "session"
        with self.assertRaises(FileNotFoundError):
            Session.create(directory, repo=tmp_path, function="f", source_path=source)
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        with Session.create(directory, repo=tmp_path, function="f", source_path=source) as s:
            assert s.status()["stats"]["nodes"] == 1

    def test_same_binary_reuses_graph_without_collapsing_source_nodes(self):
        tmp_path = self.tmp_path
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        source = tmp_path / "source.c"
        source.write_text("int f(void) { return 2; }\n")
        with Session.create(tmp_path / "session", repo=tmp_path, function="f", source_path=source) as s:
            r = s.propose("int f(void) { /* another neighborhood */ return 2; }\n",
                          hypothesis="preserve a different source representative")
            assert r["evaluation"]["graph_cached"]
            assert s.status()["stats"]["nodes"] == 2
            assert s.status()["stats"]["unique_binaries"] == 1

    def test_analysis_upgrade_reuses_bytes_and_excludes_stale_rankings(self):
        tmp_path = self.tmp_path
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        source = tmp_path / "source.c"
        source.write_text("int f(void) { return 2; }\n")
        with Session.create(tmp_path / "session", repo=tmp_path, function="f", source_path=source) as s:
            calls = s.evaluator.calls
            original_version = s.analysis_version
            s.analysis_version = "new-analysis-implementation"
            assert s.status()["stale_analysis"] == 1
            assert s.frontier() == []
            result = s.refresh()
            assert result["remaining_stale"] == 0
            assert result["refreshed"][0]["cached_compile"]
            assert s.evaluator.calls == calls
            assert len(s.frontier()) == 1
            s.analysis_version = original_version
            result = s.refresh()
            assert result["remaining_stale"] == 0
            assert s.store.node("root")["evaluation"]["analysis_version"] == original_version
            assert s.evaluator.calls == calls

    def test_source_history_crosses_contexts_without_carrying_verdicts(self):
        tmp_path = self.tmp_path
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        source = tmp_path / "source.c"
        source.write_text("int f(void) { return 2; }\n")
        previous = tmp_path / "previous"
        with Session.create(previous, repo=tmp_path, function="f", source_path=source) as s:
            match = s.propose("int f(void) { return 1; }\n", hypothesis="correct return")["id"]
            s.store.retain(match, "useful source hypothesis")
            s.store.reject("root", "historical decision")
            s.store.protect("block:0", match, "bytes", "old-context agreement")
            # A reversible graph can contain cycles, not just a topological tree.
            s.propose(s.store.source("root"), parent=match, hypothesis="test inverse")
            history = s.store.history()
            assert "code" not in history
            assert len(history["nodes"]) == 2

        class ChangedCompiler(TinyCompiler):
            def __init__(self, *args, **kwargs):
                super().__init__(*args, **kwargs)
                self.identity["compiler"] = "new-test-context"

        self.patch("tools.matchsearch.evaluator.Evaluator", ChangedCompiler)
        with Session.fork(previous, tmp_path / "current") as s:
            stats = s.status()
            assert stats["stats"]["nodes"] == 2
            assert stats["stats"]["evaluated"] == 1
            assert stats["stats"]["exact"] == 0
            assert stats["stats"]["rejected"] == 0
            assert stats["protections"] == []
            mapped = s.store.meta("origin")["node_map"][match]
            assert mapped != match
            assert s.store.node(mapped)["retained"]
            assert s.store.node(mapped)["evaluation"] is None
            assert s.store.node("root")["parents"][-1]["hypothesis"] == "test inverse"
            s.refresh()
            assert s.status()["stats"]["exact"] == 1
            assert s.status()["pending_evaluations"] == 0

    def test_baseline_call_contracts_are_not_reinferred_from_candidates(self):
        tmp_path = self.tmp_path
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        source = tmp_path / "source.c"
        source.write_text("int f(void) { return 2; }\n")
        directory = tmp_path / "session"
        with Session.create(directory, repo=tmp_path, function="f", source_path=source) as s:
            frozen = s.call_contracts
            s.propose("int g(int a, int b); int f(void) { return 1; }\n", hypothesis="candidate-only declaration")
            assert s.call_contracts == frozen
        source.write_text("int h(int a); int f(void) { return 2; }\n")
        with Session(directory) as s:
            assert s.call_contracts == frozen

    def test_history_import_retries_an_interrupted_toolchain_setup(self):
        tmp_path = self.tmp_path
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        source = tmp_path / "source.c"
        source.write_text("int f(void) { return 2; }\n")
        with Session.create(tmp_path / "previous", repo=tmp_path, function="f", source_path=source) as s:
            history = s.store.history()
        def missing(*args, **kwargs):
            raise FileNotFoundError("compiler")
        self.patch("tools.matchsearch.evaluator.Evaluator", missing)
        destination = tmp_path / "imported"
        with self.assertRaises(FileNotFoundError):
            Session.from_history(destination, history, repo=tmp_path)
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        with Session.from_history(destination, history, repo=tmp_path) as s:
            assert s.status()["stats"]["nodes"] == 1

    def test_helper_edit_keeps_the_machine_target_and_records_edited_definition(self):
        tmp_path = self.tmp_path
        self.patch("tools.matchsearch.evaluator.Evaluator", TinyCompiler)
        source = tmp_path / "source.c"
        main = "int f(void) { return 2; }\n"
        source.write_text("static inline int helper(int value) { int first; int second; return 2; }\n" + main)
        with Session.create(tmp_path / "session", repo=tmp_path, function="f", source_path=source) as s:
            assert not s.moves(families=["declaration_order"])["moves"]
            slate = s.moves(edit_function="helper", families=["declaration_order"])
            assert slate["catalog"]["function"] == "helper"
            assert len(slate["moves"]) == 1
            result = s.expand(edit_function="helper", families=["declaration_order"],
                              ids=[slate["moves"][0]["id"]],
                              hypothesis="Test an inline helper's declaration order under the same outer machine target")
            node = result["results"][0]["id"]
            assert s.store.source(node).endswith(main)
            assert re.search(r"(?:static inline|inline static) int helper", s.store.source(node))
            assert s.evaluator.function == "f"
            assert s.store.node(node)["parents"][0]["metadata"]["edited_function"] == "helper"


if __name__ == "__main__":
    unittest.main()
