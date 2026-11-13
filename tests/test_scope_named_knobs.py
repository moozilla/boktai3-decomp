"""Pseudo knobs must resolve C names rather than silently sending atoi a name."""
from pathlib import Path
import unittest
from unittest import mock

from tools.matchsearch.evaluator import _load_scope


class NamedKnobs(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def setUp(self):
        self.environ = mock.patch.dict(self.scope.os.environ, {}, clear=True)
        self.environ.start()
        self.addCleanup(self.environ.stop)

    def resolve(self, knobs, names=None):
        with mock.patch.object(self.scope, "compile_draft") as compile_draft, \
                mock.patch.object(self.scope, "workdir", return_value="scratch"), \
                mock.patch.object(self.scope, "pseudo_names", return_value=names or {32: "base", 33: "resource"}):
            result = self.scope.resolve_knobs("draft.c", "Draw", knobs)
        return result, compile_draft

    def test_unprefixed_knob_is_rejected_before_compiling(self):
        for key in ("FORCE", "PRI", "NOTIE", "COMBKEEP", "NOEQUIV", "NOREG", "MOVE"):
            with self.subTest(key=key):
                with self.assertRaisesRegex(SystemExit, f"use AGBCC_{key}"):
                    self.resolve({key: "32"})

    def test_combine_keep_resolves_mixed_list(self):
        result, compile_draft = self.resolve({"AGBCC_COMBKEEP": "base,33", "AGBCC_LOOP_TRACE": "1"})
        self.assertEqual(result["AGBCC_COMBKEEP"], "32,33")
        compile_draft.assert_called_once_with("draft.c", "Draw", {"AGBCC_LOOP_TRACE": "1"})

    def test_other_bare_pseudo_knobs_resolve_names(self):
        for key in ("AGBCC_NOEQUIV", "CSE_KEEP", "CSE_NOCANON"):
            with self.subTest(key=key):
                result, _ = self.resolve({key: "resource"})
                self.assertEqual(result[key], "33")

    def test_priority_and_force_keep_their_suffixes(self):
        result, _ = self.resolve({"AGBCC_PRI": "base:+600,33:-20", "AGBCC_FORCE": "resource:ip"})
        self.assertEqual(result["AGBCC_PRI"], "32:+600,33:-20")
        self.assertEqual(result["AGBCC_FORCE"], "33:12")

    def test_numeric_request_needs_no_name_compile(self):
        result, compile_draft = self.resolve({"AGBCC_COMBKEEP": "32,33"})
        self.assertEqual(result["AGBCC_COMBKEEP"], "32,33")
        compile_draft.assert_not_called()

    def test_named_environment_knob_is_absent_during_baseline_compile(self):
        with mock.patch.dict(self.scope.os.environ, {"AGBCC_COMBKEEP": "base"}):
            with mock.patch.object(self.scope, "compile_draft") as compile_draft, \
                    mock.patch.object(self.scope, "workdir", return_value="scratch"), \
                    mock.patch.object(self.scope, "pseudo_names", return_value={32: "base"}):
                compile_draft.side_effect = lambda *args: self.assertNotIn("AGBCC_COMBKEEP", self.scope.os.environ)
                result = self.scope.resolve_knobs("draft.c", "Draw", {})
            self.assertEqual(result["AGBCC_COMBKEEP"], "32")
            self.assertEqual(self.scope.os.environ["AGBCC_COMBKEEP"], "base")

    def test_unknown_name_fails_instead_of_disabling_knob(self):
        with self.assertRaisesRegex(SystemExit, "no pseudo by that name"):
            self.resolve({"AGBCC_COMBKEEP": "missing"})

    def test_ambiguous_name_requires_number(self):
        with self.assertRaisesRegex(SystemExit, "use a number"):
            self.resolve({"AGBCC_COMBKEEP": "base"}, {32: "base", 33: "base"})


if __name__ == "__main__":
    unittest.main()
