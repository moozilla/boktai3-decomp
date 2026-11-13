"""Named pseudo knobs must reach both compiler paths instead of silently doing nothing."""
import contextlib
import io
import os
from pathlib import Path
import unittest
from unittest.mock import patch

from tools.matchsearch.evaluator import _load_scope


class NamedKnobs(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def resolve(self, knobs, names=None):
        with patch.dict(os.environ, {}, clear=True), \
             patch.object(self.scope, "compile_draft") as compile_mock, \
             patch.object(self.scope, "workdir", return_value=Path("build/test")), \
             patch.object(self.scope, "pseudo_names", return_value=names or {24: "key", 44: "counter"}), \
             contextlib.redirect_stderr(io.StringIO()):
            result = self.scope.resolve_knobs("candidate.c", "Function", knobs)
        return result, compile_mock

    def test_noequiv_resolves_comma_separated_names_and_numbers(self):
        result, compiled = self.resolve({"AGBCC_NOEQUIV": "key,44"})
        self.assertEqual(result, {"AGBCC_NOEQUIV": "24,44"})
        compiled.assert_called_once_with("candidate.c", "Function", {})

    def test_named_knobs_are_removed_together_from_baseline(self):
        result, compiled = self.resolve({"AGBCC_NOEQUIV": "key", "AGBCC_PRI": "counter:+1",
                                         "AGBCC_FORCE": "key:r2", "AGBCC_SPILL": "3:1"})
        self.assertEqual(result, {"AGBCC_NOEQUIV": "24", "AGBCC_PRI": "44:+1",
                                  "AGBCC_FORCE": "24:2", "AGBCC_SPILL": "3:1"})
        compiled.assert_called_once_with("candidate.c", "Function", {"AGBCC_SPILL": "3:1"})

    def test_numeric_noequiv_does_not_compile_a_baseline(self):
        result, compiled = self.resolve({"AGBCC_NOEQUIV": "24,44"})
        self.assertEqual(result, {"AGBCC_NOEQUIV": "24,44"})
        compiled.assert_not_called()

    def test_noreg_resolves_names_without_leaking_them_to_baseline(self):
        result, compiled = self.resolve({"AGBCC_NOREG": "key,44", "AGBCC_NOEQUIV": "key"})
        self.assertEqual(result, {"AGBCC_NOREG": "24,44", "AGBCC_NOEQUIV": "24"})
        compiled.assert_called_once_with("candidate.c", "Function", {})

    def test_numeric_noreg_does_not_compile_a_baseline(self):
        result, compiled = self.resolve({"AGBCC_NOREG": "24,44"})
        self.assertEqual(result, {"AGBCC_NOREG": "24,44"})
        compiled.assert_not_called()

    def test_unknown_name_is_an_error(self):
        with self.assertRaisesRegex(SystemExit, "no pseudo by that name"):
            self.resolve({"AGBCC_NOEQUIV": "missing"})

    def test_ambiguous_name_requires_a_number(self):
        with self.assertRaisesRegex(SystemExit, "names pseudos"):
            self.resolve({"AGBCC_NOEQUIV": "key"}, names={24: "key", 25: "key"})

    def test_environment_name_is_hidden_during_baseline_then_restored(self):
        seen = []
        with patch.dict(os.environ, {"AGBCC_NOEQUIV": "key"}, clear=True), \
             patch.object(self.scope, "compile_draft", side_effect=lambda *args: seen.append(dict(os.environ))), \
             patch.object(self.scope, "workdir", return_value=Path("build/test")), \
             patch.object(self.scope, "pseudo_names", return_value={24: "key"}), \
             contextlib.redirect_stderr(io.StringIO()):
            result = self.scope.resolve_knobs("candidate.c", "Function", {})
            self.assertEqual(os.environ["AGBCC_NOEQUIV"], "key")
        self.assertEqual(result, {"AGBCC_NOEQUIV": "24"})
        self.assertNotIn("AGBCC_NOEQUIV", seen[0])


if __name__ == "__main__":
    unittest.main()
