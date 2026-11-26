import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import reorg


@contextlib.contextmanager
def fixture(files):
    with tempfile.TemporaryDirectory() as scratch:
        root = Path(scratch)
        for name, text in files.items():
            path = root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text)
        with (patch.object(reorg, "ROOT", root),
              patch.object(reorg, "tracked_paths", return_value={Path(name) for name in files}),
              contextlib.redirect_stdout(io.StringIO())):
            yield root


class ReorgTests(unittest.TestCase):
    def test_replays_both_old_layouts_and_ignores_report_regrouping(self):
        for flat in (True, False):
            with self.subTest(flat=flat), tempfile.TemporaryDirectory() as scratch:
                root = Path(scratch)
                old_source = "src/fn/sub_08001000.c" if flat else "src/actors/mover/Sub_08001000.c"
                old_global = "include/global.h" if flat else "include/engine/shared/global.h"
                old_mover = "include/mover.h" if flat else "include/actors/mover/mover.h"
                old_extra = "include/sub_08001000.h" if flat else "include/actors/mover/extra.h"
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
                    f'#include "{old_extra.removeprefix("include/")}"\n'
                    '#include "mover.h"\nvoid Actor_Move(void) {}\n',
                    "src/scratch/sub_08002010.c": '#include "mover.h"\nint sub_08002010(void) { return 1; }\n',
                    old_global: "#define TEST_VALUE 1\n",
                    old_mover: "void Actor_Move(void);\n",
                    old_extra: "#define EXTRA 1\n",
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
                extra = f"scene/actor/{Path(old_extra).name}"
                self.assertTrue((root / "include" / extra).is_file())
                self.assertIn(f'#include "{extra}"', (root / "src/scene/actor/sub_08001000.c").read_text())
                self.assertEqual((root / "docs/example.md").read_text(),
                                 "[Source](../src/scene/actor/sub_08001000.c)\n")

    def test_flat_header_with_ambiguous_owner_requires_a_rule(self):
        with self.assertRaisesRegex(ValueError, r"include/shared\.h: ambiguous owner.*header_dirs\.csv"):
            reorg.header_destination(Path("include/shared.h"), {}, set(),
                                     {"shared": {Path("include/scene"), Path("include/ui")}}, {})

    def test_address_renames_are_simultaneous_and_keep_source_basenames(self):
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Old_Thing\n08001020,New_Thing\n",
            "src/engine/Old_Thing.c": "void Old_Thing(void) { New_Thing(); }\n",
            "include/engine/example.h": "void Old_Thing(void);\n",
            "port/example.c": "void *const target = Old_Thing__addr;\n",
            "docs/example.md": "`Old_Thing` in `src/engine/Old_Thing.c`.\n",
            "build/renames.csv": "addr,old_name,new_name\n08001000,Old_Thing,New_Thing\n08001020,New_Thing,Final_Thing\n",
        }
        with fixture(files) as root:
            table = root / "build/renames.csv"
            preview = reorg.function_renames(table)
            self.assertEqual((root / "symbols/functions.csv").read_text(), files["symbols/functions.csv"])
            self.assertEqual(reorg.function_renames(table, apply=True), preview)
            self.assertEqual(reorg.function_renames(table, apply=True), 0)
            self.assertIn("void New_Thing(void) { Final_Thing(); }", (root / "src/engine/Old_Thing.c").read_text())
            self.assertIn("void New_Thing(void);", (root / "include/engine/example.h").read_text())
            self.assertIn("New_Thing__addr", (root / "port/example.c").read_text())
            self.assertEqual((root / "docs/example.md").read_text(), "`New_Thing` in `src/engine/Old_Thing.c`.\n")

    def test_prefixes_use_owner_scope_for_every_reference(self):
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Shared_One\n08001020,Shared_Two\n",
            "src/player/one.c": "void Shared_One(void) {}\n",
            "src/npc/two.c": "void Shared_Two(void) { Shared_One(); }\n",
            "build/prefixes.csv": "old_prefix,new_prefix,scope\nShared_,Player_,src/player/\nShared_,Npc_,src/npc/\n",
        }
        with fixture(files) as root:
            table = root / "build/prefixes.csv"
            self.assertEqual(reorg.function_renames(table, prefixes=True, apply=True), 3)
            self.assertEqual(reorg.function_renames(table, prefixes=True, apply=True), 0)
            self.assertIn("void Npc_Two(void) { Player_One(); }", (root / "src/npc/two.c").read_text())

    def test_player_file_routes_update_placement_and_replay(self):
        old = "src/player/core/sub_08001000.c"
        new = "src/link/player/sub_08001000.c"
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Player_Move\n",
            "symbols/source_dirs.csv": "current_path,new_path,note\nplayer_files_core.txt,src/player/core,fixture\n",
            "symbols/source_files.csv": "original_path,current_path,rule,filename,start,end\n"
            "src/fn/sub_08001000.c,src/player/sub_08001000.c,player_files_core.txt,sub_08001000.c,08001000,08001020\n",
            "symbols/header_dirs.csv": "current_path,new_path,note\n",
            "docs/example.md": f"[Source](../{old})\n",
            old: "void Player_Move(void) {}\n",
            "build/player_moves.csv": f"file,new_path,note\n{old},src/link/player,fixture\n",
        }
        with fixture(files) as root:
            table = root / "build/player_moves.csv"
            preview = reorg.player_moves(table)
            self.assertTrue((root / old).is_file())
            self.assertEqual(reorg.player_moves(table, apply=True), preview)
            self.assertEqual(reorg.player_moves(table, apply=True), 0)
            self.assertEqual((root / new).read_text(), files[old])
            self.assertEqual((root / "docs/example.md").read_text(), f"[Source](../{new})\n")
            self.assertEqual(reorg.recorded_moves()[Path(old)], Path(new))
            self.assertEqual(reorg.migrate(), 0)

    def test_repository_agrees_with_layout_tables(self):
        plan = io.StringIO()
        with contextlib.redirect_stdout(plan):
            remaining = reorg.migrate()
        self.assertEqual(remaining, 0, plan.getvalue()[:2000])


if __name__ == "__main__":
    unittest.main()
