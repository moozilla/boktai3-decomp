"""Do not mistake register-class exclusions for live-value conflicts."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest import mock
import scope


class AllocationExplanationTests(unittest.TestCase):
    def test_extended_reload_records_and_initial_rotation(self):
        with tempfile.TemporaryDirectory() as directory:
            Path(directory, "names.txt").write_text(
                "X F 0 68 start=-1 pass=1 rotate=1 0:L43 1:A 2:A 3:A\n"
                "X F 1 91 start=1 pass=1 future=2 rotate=0 0:A 1:A 2:A\n"
                "X F 2 95 start=2 pass=1 0:A 1:A 2:A\n")
            with mock.patch.object(scope, "dump_func", return_value="F"):
                states = scope.spill_states(directory, "F")
            self.assertEqual(states[0]["regs"], [0, 1, 2, 3])
            self.assertTrue(states[0]["rotate"])
            self.assertEqual(scope.spill_shifts(states[0], 2), [2])
            self.assertFalse(states[1]["rotate"])
            self.assertEqual(scope.spill_shifts(states[1], 2), [])
            self.assertTrue(states[2]["rotate"])

    def test_stack_class_is_not_reported_as_overlapping_low_values(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "u.i.lreg").write_text("Register 22 used 16 times across 76 insns; pref STACK_REG; pointer.\n")
            (root / "thumb.h").write_text('#define REG_CLASS_NAMES {"LO_REGS", "STACK_REG"}\n'
                                         '#define REG_CLASS_CONTENTS {0x000ff, 0x02000}\n')
            decision = {"used1": set(range(16)), "sofar": set(range(8)), "someone": set(),
                        "pref": set(), "cpref": set(), "first": -1, "passno": 1,
                        "final": -1, "alt": 0}
            output = io.StringIO()
            with mock.patch.object(scope, "INSTR", str(root / "agbcc")), \
                 mock.patch.object(scope, "find_reg_view", return_value={22: [decision]}), \
                 contextlib.redirect_stdout(output):
                self.assertEqual(scope.preferred_class(directory, 22), ("STACK_REG", {13}))
                scope.explain_find_reg(directory, "F", 22, 4, 12, str,
                                       {22: ("G", 0, 100, 16, 76, 0, -1)}, ("G", 0, 100, 16, 76, 0, -1))
            self.assertIn("outside preferred class STACK_REG", output.getvalue())
            self.assertIn("not evidence of an overlapping value", output.getvalue())
            self.assertNotIn("r4 conflicts:", output.getvalue())


if __name__ == "__main__":
    unittest.main()
