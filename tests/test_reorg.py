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
    def test_replays_both_old_layouts_and_ignores_report_regrouping(self):
        for flat in (True, False):
            with self.subTest(flat=flat), tempfile.TemporaryDirectory() as scratch:
                root = Path(scratch)
                old_source = "src/fn/sub_08001000.c" if flat else "src/actors/mover/sub_08001000.c"
                old_global = "include/global.h" if flat else "include/engine/shared/global.h"
                old_mover = "include/mover.h" if flat else "include/actors/mover/mover.h"
                files = {
                    "symbols/functions.csv": "addr,name\n08001000,Actor_Move\n",
                    "symbols/source_dirs.csv": "current_path,new_path,note\n"
                    "src/actors/mover,src/scene/actor,fixture\n"
                    "src/engine/example,src/engine,fixture\n",
                    "symbols/source_files.csv": "original_path,current_path,rule,filename,start,end\n"
                    "src/fn/sub_08001000.c,src/actors/mover/sub_08001000.c,src/actors/mover,sub_08001000.c,08001000,08001020\n"
                    "src/example.c,src/engine/example/example.c,src/engine/example,example.c,08002000,08002020\n",
                    "symbols/header_dirs.csv": "current_path,new_path,note\n"
                    "include/engine/shared/global.h,include/global.h,fixture\n"
                    "include/actors/mover/mover.h,include/scene/actor/mover.h,fixture\n",
                    "symbols/progress_subsystems.csv": "name,start,end,category\nRegrouped,08001000,08001020,other\n",
                    old_source: f'#include "{old_global.removeprefix("include/")}"\n'
                    '#include "mover.h"\nvoid Actor_Move(void) {}\n',
                    "src/scratch/sub_08002010.c": '#include "mover.h"\nint sub_08002010(void) { return 1; }\n',
                    old_global: "#define TEST_VALUE 1\n",
                    old_mover: "void Actor_Move(void);\n",
                    "docs/example.md": f"[Source](../{old_source})\n",
                }
                for name, text in files.items():
                    path = root / name
                    path.parent.mkdir(parents=True, exist_ok=True)
                    path.write_text(text)
                tracked = "\0".join(files).encode()
                with (patch.object(reorg, "ROOT", root),
                      patch.object(reorg.subprocess, "check_output", return_value=tracked),
                      contextlib.redirect_stdout(io.StringIO())):
                    before = reorg.migrate()
                    self.assertGreater(before, 0)
                    self.assertTrue((root / old_source).exists())
                    self.assertEqual(reorg.migrate(apply=True), before)
                    self.assertEqual(reorg.migrate(apply=True), 0)
                self.assertFalse((root / old_source).exists())
                self.assertTrue((root / "include/global.h").exists())
                self.assertIn('#include "scene/actor/mover.h"', (root / "src/engine/sub_08002010.c").read_text())
                self.assertIn('#include "scene/actor/mover.h"', (root / "src/scene/actor/sub_08001000.c").read_text())
                self.assertEqual((root / "docs/example.md").read_text(),
                                 "[Source](../src/scene/actor/sub_08001000.c)\n")

    def test_repository_agrees_with_layout_tables(self):
        plan = io.StringIO()
        with contextlib.redirect_stdout(plan):
            remaining = reorg.migrate()
        self.assertEqual(remaining, 0, plan.getvalue()[:2000])


if __name__ == "__main__":
    unittest.main()
