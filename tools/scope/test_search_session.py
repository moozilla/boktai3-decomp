"""Search state invariants, independent of compiler or ROM availability.

Run: build/venv/bin/python -m unittest discover -s tools/scope -p test_search_session.py
"""
import concurrent.futures
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from search_session import Session, fingerprint


SOURCE = "int Example(int x) { return x + 1; }\n"
CONTEXT = {"compiler": "gcc-hash", "flags": ["-O2"], "headers": "header-hash",
           "target": "rom-bytes-hash", "evaluator": 1}


class SearchSessionTests(unittest.TestCase):
    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory(prefix="search-session-test-")
        self.addCleanup(self.scratch.cleanup)
        self.path = Path(self.scratch.name) / "search.sqlite3"
        self.session = Session(self.path)
        self.addCleanup(self.session.close)
        self.root = self.session.initialize("Example", SOURCE, CONTEXT)

    def test_resume_retains_sources_frontier_feedback_and_policy_notes(self):
        child = self.session.add(SOURCE.replace("x + 1", "1 + x"), move={"kind": "commute", "line": 1})
        feedback = {"exact": False, "different": 4, "regions": [{"start": 0, "exact": True}],
                    "semantic": {"status": "unknown"}}
        self.session.evaluate(child["id"], feedback)
        self.session.note("Operand order changes preferences", child["id"], kind="hypothesis")
        self.session.set_frontier(child["id"], "parked", reason="revisit after shape change")
        with Session(self.path) as resumed:
            self.assertEqual(resumed.source(), SOURCE.replace("x + 1", "1 + x"))
            self.assertEqual(resumed.evaluation()["result"], feedback)
            self.assertEqual(resumed.notes(child["id"])[0]["kind"], "hypothesis")
            self.assertEqual([node["id"] for node in resumed.frontier()], [self.root["id"]])
            self.assertEqual(len(resumed.frontier(include_closed=True)), 2)
            # Reopening a session must not reset HEAD or erase retained branches.
            resumed.initialize("Example", SOURCE, CONTEXT)
            self.assertEqual(resumed.node()["id"], child["id"])

    def test_transpositions_deduplicate_nodes_but_preserve_both_moves(self):
        left = self.session.add("int Example(int x) { return 1 + x; }\n", move="commute", checkout=False)
        right = self.session.add("int Example(int x) { int y = x; return y + 1; }\n", move="temporary", checkout=False)
        shared_source = "int Example(int x) { int y = x; return 1 + y; }\n"
        first = self.session.add(shared_source, left["id"], {"kind": "temporary"}, checkout=False)
        second = self.session.add(shared_source, right["id"], {"kind": "commute"}, checkout=False)
        self.assertEqual(first["id"], second["id"])
        self.assertEqual(set(second["parents"]), {left["id"], right["id"]})
        self.assertEqual(len(self.session.frontier()), 4)
        self.assertEqual(len(self.session.history(second["id"])), 4)
        self.session.evaluate(first["id"], {"exact": False, "different": 8})
        self.assertEqual(self.session.evaluation(second["id"])["result"]["different"], 8)
        # Repeating the identical edge is idempotent, but the proposal is in events.
        repeated = self.session.add(shared_source, right["id"], {"kind": "commute"}, checkout=False)
        self.assertEqual(len(repeated["edges"]), 2)
        self.assertEqual(len([e for e in self.session.events() if e["kind"] == "add"]), 5)

    def test_context_and_evaluator_options_invalidate_only_their_own_cache(self):
        self.session.evaluate("ROOT", {"exact": True})
        changed_context = dict(CONTEXT, headers="different-header-hash")
        changed = self.session.add(SOURCE, context=changed_context, move="context-refresh")
        self.assertNotEqual(changed["id"], self.root["id"])
        self.assertEqual(changed["source_hash"], self.root["source_hash"])
        self.assertIsNone(self.session.evaluation(changed["id"]))
        self.assertIsNone(self.session.evaluation("ROOT", options={"AGBCC_NOPRE": "1"}))
        self.assertIsNone(self.session.evaluation("ROOT", evaluator="compile-v2"))
        self.session.evaluate("ROOT", {"exact": False}, options={"AGBCC_NOPRE": "1"})
        self.assertTrue(self.session.evaluation("ROOT")["result"]["exact"])
        self.assertFalse(self.session.evaluation("ROOT", options={"AGBCC_NOPRE": "1"})["result"]["exact"])
        self.assertEqual(self.session.context(changed["id"]), changed_context)
        # Dict ordering does not force a pointless compile.
        self.assertEqual(fingerprint(CONTEXT), fingerprint(dict(reversed(list(CONTEXT.items())))))

    def test_equal_worse_failed_and_unknown_semantic_results_are_all_retained(self):
        for index, result in enumerate([
                {"exact": False, "different": 2, "semantic": "unknown"},
                {"exact": False, "different": 80, "semantic": "pass"},
                {"compile_error": "undeclared identifier", "semantic": "unknown"},
                {"exact": True, "semantic": "fail"}]):
            node = self.session.add(SOURCE + f"/* branch {index} */\n", parent="ROOT", move="edit")
            self.session.evaluate(node["id"], result)
        self.assertEqual(len(self.session.frontier()), 5)
        self.assertEqual(len(self.session.export()["nodes"]), 5)
        # A policy, never evaluate(), decides whether semantic failure closes a branch.
        self.assertEqual(self.session.node()["frontier"]["state"], "open")

    def test_undo_cycle_is_finite_and_parent_sources_never_change(self):
        child = self.session.add(SOURCE.replace("+ 1", "+ 2"), move="edit")
        undone = self.session.add(SOURCE, move="undo")
        self.assertEqual(undone["id"], self.root["id"])
        self.assertEqual(len(self.session.history()), 2)
        self.assertEqual(self.session.source(child["id"]), SOURCE.replace("+ 1", "+ 2"))
        self.assertEqual(self.session.source("ROOT"), SOURCE)

    def test_invalid_move_rolls_back_source_edge_and_head_together(self):
        before = self.session.export()
        with self.assertRaises(TypeError):
            self.session.add(SOURCE + "/* must roll back */", move={"bad": object()})
        after = self.session.export()
        self.assertEqual(after, before)
        with self.assertRaises(ValueError):
            self.session.initialize("Other", SOURCE, CONTEXT)
        self.assertEqual(self.session.node()["id"], self.root["id"])

    def test_checkout_diff_history_and_export(self):
        variant = SOURCE.replace("x + 1", "1 + x")
        child = self.session.add(variant, move="commute")
        delta = self.session.diff("ROOT", child["id"][:9])
        self.assertIn("-" + SOURCE.rstrip(), delta)
        self.assertIn("+" + variant.rstrip(), delta)
        destination = Path(self.scratch.name) / "checkout" / "candidate.c"
        self.session.checkout("ROOT", destination)
        self.assertEqual(destination.read_text(), SOURCE)
        self.assertEqual(self.session.node()["id"], self.root["id"])
        output = Path(self.scratch.name) / "export.json"
        snapshot = self.session.export(output)
        self.assertEqual(json.loads(output.read_text()), snapshot)
        self.assertEqual(len(snapshot["sources"]), 2)
        self.assertEqual(len(snapshot["edges"]), 1)

    def test_feedback_re_evaluations_have_an_audit_trail(self):
        one = self.session.evaluate("ROOT", {"exact": False, "elapsed_ms": 12})
        two = self.session.evaluate("ROOT", {"exact": False, "elapsed_ms": 9})
        self.assertNotEqual(one["seq"], two["seq"])
        self.assertEqual(self.session.evaluation("ROOT")["seq"], two["seq"])
        self.assertEqual(len(self.session.node("ROOT")["evaluations"]), 2)

    def test_concurrent_branch_additions_deduplicate_without_lost_events(self):
        root_id = self.root["id"]
        def worker(index):
            with Session(self.path) as session:
                branch = session.add(SOURCE + "/* shared program */\n", parent=root_id,
                                     move={"kind": "edit", "worker": index}, checkout=False)
                session.note(f"Worker {index} observed this", branch["id"])
                return branch["id"]
        with concurrent.futures.ThreadPoolExecutor(max_workers=6) as pool:
            keys = list(pool.map(worker, range(12)))
        self.assertEqual(len(set(keys)), 1)
        self.assertEqual(len(self.session.node(keys[0])["edges"]), 12)
        self.assertEqual(len(self.session.notes(keys[0])), 12)
        self.assertEqual(len(self.session.frontier()), 2)
        self.assertEqual(len([e for e in self.session.events() if e["kind"] == "add"]), 12)

    def test_claim_lease_prevents_duplicate_work_and_recovers_from_crash(self):
        with Session(self.path) as other:
            with patch("search_session.time.time", return_value=100):
                self.assertTrue(self.session.claim("ROOT", owner="one", ttl=10))
                self.assertFalse(other.claim("ROOT", owner="two", ttl=10))
                other.release("ROOT", owner="two")  # Cannot release another worker's lease.
                self.assertFalse(other.claim("ROOT", owner="two", ttl=10))
            with patch("search_session.time.time", return_value=111):
                self.assertTrue(other.claim("ROOT", owner="two", ttl=10))
                other.evaluate("ROOT", {"exact": False})
                self.assertFalse(self.session.claim("ROOT", owner="one"))

    def test_default_lease_owner_is_unique_to_each_worker(self):
        with Session(self.path) as other:
            self.assertTrue(self.session.claim("ROOT"))
            self.assertFalse(other.claim("ROOT"))
            other.release("ROOT")
            self.assertFalse(other.claim("ROOT"))
            self.session.release("ROOT")
            self.assertTrue(other.claim("ROOT"))


if __name__ == "__main__":
    unittest.main()
