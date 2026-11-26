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

    def test_replays_legacy_links_after_a_source_basename_changes(self):
        legacy = "src/fn/sub_08001000.c"
        earlier = "src/scene/actor/sub_08001000.c"
        active = "src/scene/actor/Actor_Move.c"
        guide = "docs/example.md"
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Actor_Move\n",
            "symbols/source_dirs.csv": "current_path,new_path,note\n"
            "src/actors/mover,src/scene/actor,fixture\n",
            "symbols/source_files.csv": "original_path,current_path,rule,filename,start,end\n"
            f"{legacy},src/actors/mover/sub_08001000.c,src/actors/mover,sub_08001000.c,08001000,08001020\n",
            "symbols/header_dirs.csv": "current_path,new_path,note\n",
            active: "void Actor_Move(void) {}\n",
            guide: f"[Flat](../{legacy}) and [Nested](../{earlier}).\n",
        }
        with fixture(files) as root:
            self.assertEqual(reorg.migrate(), 1)
            self.assertEqual((root / guide).read_text(), files[guide])
            self.assertEqual(reorg.migrate(apply=True), 1)
            self.assertEqual((root / guide).read_text(), f"[Flat](../{active}) and [Nested](../{active}).\n")
            self.assertEqual((root / active).read_text(), files[active])
            self.assertEqual(reorg.migrate(), 0)

    def test_address_renames_are_simultaneous_and_keep_source_basenames(self):
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Old_Thing\n08001020,New_Thing\n",
            "src/engine/Old_Thing.c": "void Old_Thing(void) { New_Thing(); }\n",
            "include/engine/example.h": "void Old_Thing(void);\n",
            "port/example.c": "void *const target = Old_Thing__addr;\n",
            "docs/example.md": "`Old_Thing` in `src/engine/Old_Thing.c`.\n",
            "docs/history/example.md": "The unit called `Old_Thing`.\n",
            "gcl/names/example.csv": "name\nOld_Thing\n",
            "docs/attempts.tsv": "address\tnote\n08001000\tOld_Thing\n",
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
            self.assertEqual((root / "docs/history/example.md").read_text(), files["docs/history/example.md"])
            self.assertEqual((root / "gcl/names/example.csv").read_text(), "name\nNew_Thing\n")
            self.assertEqual((root / "docs/attempts.tsv").read_text(), "address\tnote\n08001000\tNew_Thing\n")

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

    def test_exact_replay_accepts_prior_names_and_reports_branch_names(self):
        table_path = "symbols/function_renames.csv"
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Interim_Run\n08001020,Branch_Run\n",
            "src/engine/run.c": "void Interim_Run(void) { Branch_Run(); }\n",
            table_path: "addr,old_name,new_name,prior_names\n"
            "08001000,Old_Run,Final_Run,Interim_Run\n"
            "08001020,Other_Run,Reviewed_Run,\n",
        }
        with fixture(files) as root, contextlib.redirect_stderr(io.StringIO()) as messages:
            table = root / table_path
            with self.assertRaisesRegex(ValueError, "08001020.*Branch_Run"):
                reorg.function_renames(table, apply=True)
            self.assertEqual(reorg.function_renames(table, apply=True, skip_diverged=True), 2)
            self.assertIn("skip", messages.getvalue())
            self.assertIn("08001020 is 'Branch_Run'", messages.getvalue())
            self.assertIn("void Final_Run(void) { Branch_Run(); }", (root / "src/engine/run.c").read_text())
            self.assertEqual(reorg.function_renames(table, skip_diverged=True), 0)

    def test_settled_references_update_tooling_without_reusing_active_names(self):
        table_path = "symbols/function_renames.csv"
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Final_Run\n08001020,InUse\n08001040,Other_Run\n",
            table_path: "addr,old_name,new_name,prior_names\n"
            "08001000,Old_Run,Final_Run,Interim_Run\n08001040,InUse,Other_Run,\n",
            "tools/example.js": "Old_Run(); Interim_Run__addr; InUse(); Old_Runner();\n",
            "port/example.c": "void Old_Run(void);\n",
            "tests/example.py": "target = 'Interim_Run'\n",
            "src/engine/Old_Run.c": "void Final_Run(void) {} /* Old_Run */\n",
        }
        with fixture(files) as root, contextlib.redirect_stderr(io.StringIO()) as messages:
            table = root / table_path
            self.assertEqual(reorg.function_renames(table), 0)
            self.assertEqual(reorg.function_renames(table, apply=True, settled_references=True), 3)
            self.assertEqual(reorg.function_renames(table, settled_references=True), 0)
            self.assertIn("retired spelling InUse is active", messages.getvalue())
            self.assertEqual((root / "tools/example.js").read_text(),
                             "Final_Run(); Final_Run__addr; InUse(); Old_Runner();\n")
            self.assertEqual((root / "src/engine/Old_Run.c").read_text(), files["src/engine/Old_Run.c"])

    def test_scoped_helpers_and_types_do_not_change_another_owner(self):
        player, npc = "src/player/local.c", "src/npc/local.c"
        body = ("struct SharedSprite;\nstatic void Shared_Helper(struct SharedSprite *it) {}\n"
                "static void Shared_Step(void) {}\n")
        files = {
            player: body,
            npc: body,
            "symbols/functions.csv": "addr,name\n",
            "symbols/prefix_renames.csv": "old_prefix,new_prefix,scope\nShared_,Player_,src/player/\n"
            "Shared_Helper,Player_HandleSprite,src/player/local.c\n",
            "symbols/type_renames.csv": "old_name,new_name,scope\nSharedSprite,PlayerSprite,src/player/\n",
        }
        with fixture(files) as root:
            prefixes = root / "symbols/prefix_renames.csv"
            types = root / "symbols/type_renames.csv"
            self.assertEqual(reorg.function_renames(prefixes, prefixes=True, apply=True), 1)
            self.assertEqual(reorg.type_renames(types, apply=True), 1)
            self.assertEqual(reorg.function_renames(prefixes, prefixes=True), 0)
            self.assertEqual(reorg.type_renames(types), 0)
            self.assertEqual((root / npc).read_text(), body)
            self.assertIn("Player_HandleSprite(struct PlayerSprite *it)", (root / player).read_text())
            self.assertIn("Player_Step(void)", (root / player).read_text())

    def test_prefix_replay_reports_collisions_and_leaves_audit_records(self):
        path = "src/engine/probe.c"
        audit = ["symbols/identities.csv", "symbols/identity_corrections.csv", "notes/identities.md"]
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Old_Move\n08001020,New_Move\n08001040,Old_Step\n",
            path: "struct OldBody;\nvoid Old_Move(void) {}\nvoid Old_Step(void) { Old_Move(); }\n",
            "symbols/prefix_renames.csv": "old_prefix,new_prefix,scope\nOld_,New_,src/engine/\n",
            **{name: "Old_Step\n" for name in audit},
        }
        with fixture(files) as root, contextlib.redirect_stderr(io.StringIO()) as messages:
            table = root / "symbols/prefix_renames.csv"
            self.assertEqual(reorg.function_renames(table, prefixes=True, apply=True,
                                                   skip_collisions=True, warn_types=True), 2)
            self.assertIn("Old_Move -> New_Move would collide", messages.getvalue())
            self.assertIn("check legacy type OldBody", messages.getvalue())
            self.assertIn("void New_Step(void) { Old_Move(); }", (root / path).read_text())
            for name in audit:
                self.assertEqual((root / name).read_text(), files[name])

    def test_type_renames_keep_related_identifiers_and_the_input_table_intact(self):
        path = "include/engine/video/sprite.h"
        table_path = "symbols/type_renames.csv"
        files = {
            path: "struct OldSprite;\nstruct OldSpriteState;\nvoid OldSprite_Draw(struct OldSprite *p);\n",
            table_path: "old_name,new_name\nOldSprite,NewSprite\nOldSprite_Draw,NewSprite_Draw\n",
        }
        with fixture(files) as root:
            table = root / table_path
            self.assertEqual(reorg.type_renames(table, apply=True), 1)
            self.assertEqual(reorg.type_renames(table), 0)
            self.assertEqual((root / table_path).read_text(), files[table_path])
            self.assertEqual((root / path).read_text(),
                             "struct NewSprite;\nstruct OldSpriteState;\nvoid NewSprite_Draw(struct NewSprite *p);\n")

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

    def test_filename_plan_covers_multi_function_units_and_pending_patches(self):
        folder = "src/world/monster"
        multi = Path(folder, "sub_08001000.c")
        named = Path(folder, "sub_08001020.c")
        review = Path(folder, "legacy.c")
        unnamed = Path(folder, "sub_08001040.c")
        tilemap = Path(folder, "wrapped_tilemap_cells.c")
        metadata = (f"diff --git a/{multi} b/{multi}\n--- a/{multi}\n+++ b/{multi}\n"
                    f"rename from {multi}\nrename to {multi}\n")
        patch_paths = [f"tools/{tool}/pending_src_fixes/draft.patch"
                       for tool in ("autopilot", "testbench")]
        files = {
            "symbols/functions.csv": "addr,name\n08001000,Monster_Init\n08001010,Monster_Update\n"
                                     "08001020,Monster_Draw_08001020\n08001030,Monster_Use\n",
            "symbols/source_dirs.csv": f"current_path,new_path,note\n{folder},{folder},fixture\n",
            "symbols/source_files.csv": "original_path,current_path,rule,filename,start,end,planned_basename\n"
                                        f"{multi},{multi},{folder},{multi.name},08001000,08001020,\n"
                                        f"{named},{named},{folder},{named.name},08001020,08001030,\n"
                                        f"{review},{review},{folder},{review.name},08001030,08001040,overview.c\n"
                                        f"{unnamed},{unnamed},{folder},{unnamed.name},08001040,08001050,\n"
                                        f"{tilemap},{tilemap},{folder},{tilemap.name},08001050,08001060,\n",
            "symbols/header_dirs.csv": "current_path,new_path,note\n",
            str(multi): "void Monster_Init(void) {}\nvoid Monster_Update(void) {}\n",
            str(named): "void Monster_Draw_08001020(void) {}\n",
            str(review): "void Monster_Use(void) {}\n",
            str(unnamed): "void sub_08001040(void) {}\n",
            str(tilemap): "void sub_08001050(void) {}\n",
            **dict.fromkeys(patch_paths, metadata),
        }
        with fixture(files) as root:
            manifest = reorg.scan(root)
            recorded = reorg.recorded_moves()
            self.assertEqual(reorg.plan_files(recorded, False, manifest)[multi], multi)
            plan = reorg.plan_files(recorded, True, manifest)
            self.assertEqual(plan[multi], multi.with_name("Monster_Init.c"))
            self.assertEqual(plan[named], named.with_name("Monster_Draw_08001020.c"))
            self.assertEqual(plan[review], review.with_name("overview.c"))
            self.assertEqual(plan[unnamed], unnamed)
            self.assertEqual(plan[tilemap], tilemap)
            for path in patch_paths:
                transformed = reorg.transform(Path(path), metadata, plan, {})
                self.assertEqual(transformed, metadata.replace(str(multi), str(plan[multi])))
            self.assertTrue((root / multi).is_file())

    def test_repository_agrees_with_layout_tables(self):
        plan = io.StringIO()
        with contextlib.redirect_stdout(plan):
            remaining = reorg.migrate()
        self.assertEqual(remaining, 0, plan.getvalue()[:2000])


if __name__ == "__main__":
    unittest.main()
