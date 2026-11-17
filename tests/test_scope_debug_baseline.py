"""A debug-mode baseline comparison must not inherit diagnostic controls."""
from pathlib import Path
import tempfile
import unittest
from unittest import mock

from tools.matchsearch.evaluator import _load_scope


class DebugBaseline(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.source = Path(self.directory.name) / "baseline.c"
        self.source.write_text("void Baseline(void) {}\n")
        self.knobs = {"AGBCC_NOMOVE": "132", "CSE_NOFOLLOW_JUMP": "9",
                      "AGBCC_PRI": "24:+100"}
        self.config = {"AGBCC_SERVER": "7:8", "AGBCC_DUMPPRI": "names.txt",
                       "AGBCC_EXTRA": "-O2 -mthumb-interwork", "SCOPE_SERVER": "0"}
        environ = mock.patch.dict(self.scope.os.environ,
                                  {**self.knobs, **self.config}, clear=True)
        environ.start()
        self.addCleanup(environ.stop)
        memo = mock.patch.object(self.scope, "_MEMO", {})
        memo.start()
        self.addCleanup(memo.stop)

    def compare(self):
        return self.scope.g_sensitive(str(self.source), "Baseline")

    def test_both_baselines_ignore_inherited_knobs(self):
        calls = []

        def compile_baseline(src, func, env, plain):
            # Use the real resolver: an empty explicit dict alone does not
            # exclude shell controls from either compiler mode.
            resolved = self.scope.resolve_knobs(src, func, env)
            calls.append((plain, resolved, dict(self.scope.os.environ)))
            code = (b"plain" if plain else b"debug") if resolved else b"same"
            return code, 0, []

        with mock.patch.object(self.scope, "fast_compile", side_effect=compile_baseline):
            self.assertFalse(self.compare())
        self.assertEqual([(plain, knobs) for plain, knobs, _ in calls],
                         [(True, {}), (False, {})])
        for _, _, environ in calls:
            self.assertEqual(environ, self.config)
        self.assertEqual(dict(self.scope.os.environ), {**self.knobs, **self.config})

    def test_real_debug_difference_is_still_reported(self):
        def compile_baseline(src, func, env, plain):
            self.assertEqual(self.scope.resolve_knobs(src, func, env), {})
            return (b"plain" if plain else b"debug"), 0, []

        with mock.patch.object(self.scope, "fast_compile", side_effect=compile_baseline):
            self.assertTrue(self.compare())

    def test_knobs_are_restored_if_either_compile_raises(self):
        for failure_mode in (True, False):
            with self.subTest(plain=failure_mode):
                def compile_baseline(src, func, env, plain):
                    self.assertEqual(self.scope.resolve_knobs(src, func, env), {})
                    if plain == failure_mode:
                        raise RuntimeError("compile failed")
                    return b"same", 0, []

                with mock.patch.object(self.scope, "fast_compile", side_effect=compile_baseline):
                    with self.assertRaisesRegex(RuntimeError, "compile failed"):
                        self.compare()
                self.assertEqual(dict(self.scope.os.environ),
                                 {**self.knobs, **self.config})
                self.assertEqual(self.scope._MEMO, {})


if __name__ == "__main__":
    unittest.main()
