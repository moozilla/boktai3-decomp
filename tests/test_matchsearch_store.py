"""Search integrity: transpositions, context isolation, concurrency, and plateaus."""
from __future__ import annotations

from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import sqlite3
import subprocess
import sys

import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.matchsearch.frontier import select
from tools.matchsearch.store import ContextChanged, Store


def _row(name, distance, *, retained=False, binary=None, expansions=0):
    return {"id": name, "label": name, "created": distance, "expansions": expansions,
            "retained": retained, "retention_reason": "struct hypothesis", "rejected": "",
            "evaluation": {"exact": False, "code_hash": binary or name,
                "structure_hash": name, "instruction_diff": distance, "candidate_size": 100 + distance,
                "target_size": 100, "concessions": {},
                "distances": {"dependency": distance, "control": distance, "operations": distance}}}


class StoreIntegrityTests(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.tmp_path = Path(directory.name)

    def patch(self, target, value):
        replacement = patch(target, value)
        self.addCleanup(replacement.stop)
        return replacement.start()

    def test_transpositions_keep_both_histories_and_equal_binary_sources(self):
        tmp_path = self.tmp_path
        with Store(tmp_path) as s:
            s.bind_context({"compiler": "real", "rom": "target"})
            a, _ = s.add("int f(void) { return 1; }", label="a")
            b, _ = s.add("int f(void) { return 1 + 0; }", parent=a, move_id="add-zero")
            c, _ = s.add("int f(void) { return 0 + 1; }", parent=a, move_id="commute")
            b_again, created = s.add(s.source(b), parent=c, move_id="fold")
            assert b_again == b and not created
            assert {p["parent"] for p in s.node(b)["parents"]} == {a, c}
            for n in (a, b, c):
                s.record(n, {"code_hash": "same", "exact": False})
            assert s.stats()["nodes"] == 3
            assert s.stats()["unique_binaries"] == 1
        with Store(tmp_path) as reopened:
            assert reopened.source(b) == "int f(void) { return 1 + 0; }"
            assert len(reopened.node(b)["parents"]) == 2

    def test_wrong_context_and_diagnostic_matches_cannot_contaminate_verdict(self):
        tmp_path = self.tmp_path
        with Store(tmp_path) as s:
            s.bind_context({"compiler": "one", "rom": "target"})
            n, _ = s.add("source")
            with self.assertRaises(ContextChanged):
                s.bind_context({"compiler": "two", "rom": "target"})
            assert s.meta("context")["compiler"] == "one"
            with self.assertRaises(ValueError):
                s.record(n, {"exact": True, "diagnostic_knobs": {"AGBCC_FORCE": "1:2"}})
            s.add_evidence(n, "counterfactual", {"diagnostic_match": True})
            assert s.node(n)["evaluation"] is None
            assert s.stats()["exact"] == 0

    def test_claim_race_has_one_owner_and_expired_worker_is_fenced(self):
        tmp_path = self.tmp_path
        with Store(tmp_path) as s:
            s.bind_context({"compiler": "one"})
        def claim():
            with Store(tmp_path) as s:
                return s.claim("same-move")
        with ThreadPoolExecutor(max_workers=6) as workers:
            results = list(workers.map(lambda _: claim(), range(6)))
        assert sum(r is not None for r in results) == 1
        old = next(r for r in results if r is not None)
        with Store(tmp_path) as s:
            s.db.execute("UPDATE claims SET expires=0 WHERE key=?", (old.key,))
            current = s.claim(old.key)
            assert current.generation == old.generation + 1
            assert not s.finish_claim(old, {"stale": True})
            assert s.finish_claim(current, {"correct": True})
            assert s.claim(old.key) is None

    def test_expired_evaluator_cannot_overwrite_new_workers_result(self):
        tmp_path = self.tmp_path
        with Store(tmp_path) as s:
            s.bind_context({"compiler": "one"})
            node, _ = s.add("source")
            old = s.claim("evaluation")
            s.db.execute("UPDATE claims SET expires=0")
            new = s.claim("evaluation")
            assert s.record(node, {"exact": False}, claim=new)
            assert not s.record(node, {"exact": True}, claim=old)
            assert not s.node(node)["evaluation"]["exact"]

    def test_retained_worse_branch_survives_and_exploration_leaves_local_minimum(self):
        rows = [_row("best", 1, expansions=30), _row("detour", 50, retained=True), _row("other", 90)]
        assert "detour" in {r["id"] for r in select(rows, limit=2)}
        seen = set()
        for epoch in range(6):
            seen.update(r["id"] for r in select(rows, limit=3, epoch=epoch))
        assert seen == {"best", "detour", "other"}

    def test_same_binary_sources_are_still_selectable(self):
        rows = [_row("same-a", 5, binary="one"), _row("same-b", 5, binary="one")]
        assert {r["id"] for r in select(rows)} == {"same-a", "same-b"}

    def test_short_source_does_not_outweigh_better_machine_constraints(self):
        better, shorter = _row("better", 1), _row("shorter", 2)
        better["evaluation"]["source_complexity"] = 10000
        shorter["evaluation"]["source_complexity"] = 10
        assert select([better, shorter], limit=1)[0]["id"] == "better"

    def test_large_reanalysis_payloads_survive_process_restart_and_checked_backup(self):
        tmp_path = self.tmp_path
        # A real regression shape: large JSON overflow pages are replaced by an
        # analyzer upgrade, then another process reopens the graph.
        with Store(tmp_path / "session") as s:
            s.bind_context({"compiler": "one"})
            nodes = [s.add(f"source {i}")[0] for i in range(6)]
            for generation in range(3):
                for i, node in enumerate(nodes):
                    s.record(node, {"generation": generation, "exact": False},
                             {"payload": (f"{generation}:{i}|" * (170_000 - generation * 21_001))})
            assert s.integrity()["ok"]
            assert s.integrity()["journal_mode"] == "delete"
            backup = tmp_path / "checkpoint.sqlite3"
            assert s.backup(backup)["integrity"] == "ok"
            with self.assertRaises(FileExistsError):
                s.backup(backup)
            s.record(nodes[0], {"generation": 99}, {"payload": "later"})
        check = subprocess.run([sys.executable, "-c", """
import json, sqlite3, sys
c = sqlite3.connect('file:' + sys.argv[1] + '?mode=ro', uri=True)
assert c.execute('PRAGMA quick_check').fetchall() == [('ok',)]
assert c.execute('PRAGMA journal_mode').fetchone()[0] == 'delete'
rows = c.execute('SELECT result,analysis FROM evaluations').fetchall()
assert len(rows) == 6
for result, analysis in rows:
    assert json.loads(result)['generation'] == 2
    assert len(json.loads(analysis)['payload']) == 4 * (170000 - 2 * 21001)
c.close()
""", str(backup)], capture_output=True, text=True)
        assert check.returncode == 0, check.stderr
        with Store(tmp_path / "session") as reopened:
            assert reopened.integrity()["ok"]
            assert reopened.node(nodes[0])["evaluation"]["generation"] == 99
        assert not list((tmp_path / "session").glob('*-wal'))

    def test_healthy_quiescent_wal_database_migrates_without_losing_graph(self):
        tmp_path = self.tmp_path
        with Store(tmp_path) as s:
            s.bind_context({"compiler": "one"})
            node, _ = s.add("source")
        c = sqlite3.connect(tmp_path / "search.sqlite3")
        assert c.execute("PRAGMA journal_mode=WAL").fetchone()[0] == "wal"
        c.execute("UPDATE nodes SET label='saved-before-migration'")
        c.commit()
        c.close()
        with Store(tmp_path) as reopened:
            assert reopened.integrity()["ok"]
            assert reopened.integrity()["journal_mode"] == "delete"
            assert reopened.node(node)["label"] == "saved-before-migration"


if __name__ == "__main__":
    unittest.main()
