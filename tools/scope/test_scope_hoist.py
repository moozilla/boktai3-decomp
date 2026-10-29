"""Optional source probes must not discard the compiler's invariant evidence."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest import mock

import scope


class HoistProbeTests(unittest.TestCase):
    def test_nested_shadowed_local_is_not_moved_out_of_scope(self):
        source = '''void F(void) {
    int banks;
    while (1) {
        if (1) {
            int banks;
            banks = 3;
        }
    }
}
'''
        close, reason = scope.hoist_probe_close(source, 6)
        self.assertIsNone(close)
        self.assertIn("scope", reason)

    def test_actual_enclosing_loop_ignores_earlier_loops_and_literal_braces(self):
        source = '''void F(void) {
    int banks;
    while (1) { break; }
    while (1) {
        /* } */ banks = 3;
        consume("}");
    }
}
'''
        self.assertEqual(scope.hoist_probe_close(source, 5), (6, None))

    def test_macro_or_unbraced_loop_is_explicitly_unavailable(self):
        source = '''void F(void) {
    int banks;
    while (1)
        banks = 3;
}
'''
        self.assertIsNone(scope.hoist_probe_close(source, 4)[0])

    def test_failed_speculative_compile_is_reported_without_aborting(self):
        source = '''void F(void) {
    int banks;
    while (1) {
        banks = 3;
    }
}
'''
        passes = [{1: {20: {"reg": 17, "move": (True, "moved", 3)}}}]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "draft.c").write_text(source)
            output = io.StringIO()
            with mock.patch.object(scope, "ROOT", str(root)), \
                 mock.patch.object(scope, "WORK", str(root / "run")), \
                 mock.patch.object(scope, "score", side_effect=SystemExit("COMPILE FAILED: unavailable local")), \
                 contextlib.redirect_stdout(output):
                scope.hoist_levers("draft.c", "F", {}, passes, {17: "banks"}, {20: 4}, 9, {})
            self.assertIn("lever unavailable", output.getvalue())
            self.assertIn("speculative source did not compile", output.getvalue())


if __name__ == "__main__":
    unittest.main()
