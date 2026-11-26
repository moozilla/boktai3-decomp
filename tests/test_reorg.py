import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import reorg


class ReorgTests(unittest.TestCase):
    def test_directory_replay_rewrites_paths_and_is_idempotent(self):
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            files = {
                "symbols/functions.csv": "addr,name\n08001000,Actor_Move\n",
                "symbols/progress_categories.csv": "id,name,start,end,name_regex,evidence\nactors,Actors,,,^Mover$,test\n",
                "symbols/progress_subsystems.csv": "name,start,end,category\nMover,08001000,08001020,actors\n",
                "symbols/progress_libraries.csv": "name,start,end\n",
                "src/fn/sub_08001000.c": '#include "mover.h"\nvoid Actor_Move(void) {}\n',
                "include/mover.h": "void Actor_Move(void);\n",
                "docs/example.md": "[Source](../src/fn/sub_08001000.c)\n",
            }
            for name, text in files.items():
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(text)
            tracked = '\0'.join(files).encode()
            with (patch.object(reorg, "ROOT", root),
                  patch.object(reorg.subprocess, "check_output", return_value=tracked),
                  contextlib.redirect_stdout(io.StringIO())):
                self.assertEqual(reorg.migrate(), 3)
                self.assertTrue((root / "src/fn/sub_08001000.c").exists())
                self.assertEqual(reorg.migrate(apply=True), 3)
                self.assertEqual(reorg.migrate(apply=True), 0)
            self.assertFalse((root / "include/mover.h").exists())
            self.assertIn('#include "actors/mover/mover.h"',
                          (root / "src/actors/mover/sub_08001000.c").read_text())
            self.assertEqual((root / "docs/example.md").read_text(),
                             "[Source](../src/actors/mover/sub_08001000.c)\n")


if __name__ == "__main__":
    unittest.main()
