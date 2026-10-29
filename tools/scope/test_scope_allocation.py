"""Do not mistake register-class exclusions for live-value conflicts."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest import mock
import scope


class AllocationExplanationTests(unittest.TestCase):
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
