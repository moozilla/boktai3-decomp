"""Hoist levers must not interpret a header's line as a caller edit site."""
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from tools.matchsearch.evaluator import _load_scope


class HoistSource(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_uid_locations_keep_header_and_caller(self):
        with tempfile.TemporaryDirectory() as directory:
            Path(directory, "u.i.rtl").write_text(
                '(note 1 0 2 ("include/step.h") 90)\n\n'
                '(insn/i 2 1 3 (set (reg:SI 22) (const_int 1)))\n\n'
                '(note 3 2 4 ("caller.c") 8)\n\n'
                '(insn 4 3 0 (set (reg:SI 23) (const_int 2)))\n')
            self.assertEqual(self.scope.uid_locations(directory),
                             {2: ("include/step.h", 90), 4: ("caller.c", 8)})
            self.assertEqual(self.scope.uid_lines(directory), {2: 90, 4: 8})

    def check_header_site(self, line):
        with tempfile.TemporaryDirectory() as directory:
            Path(directory, "caller.c").write_text(
                'for (;;) {\n    bit = 1;\n    use(bit);\n}\n')
            passes = [{1: {42: {"reg": 22, "move": (True,)}}}]
            with patch.object(self.scope, "ROOT", directory), \
                 patch.object(self.scope, "uid_locations", return_value={42: ("include/step.h", line)}), \
                 patch.object(self.scope, "score") as score:
                self.scope.hoist_levers("caller.c", "Update", {}, passes,
                                        {22: "bit"}, {42: line}, 1, {"wd": directory})
                score.assert_not_called()

    def test_header_line_outside_caller(self):
        self.check_header_site(90)

    def test_header_line_overlaps_caller_assignment(self):
        self.check_header_site(2)


if __name__ == "__main__":
    unittest.main()
