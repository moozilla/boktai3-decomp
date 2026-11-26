import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import source_layout


class SourceLayoutTests(unittest.TestCase):
    def test_slug_keeps_hex_and_acronyms_together(self):
        self.assertEqual(source_layout.slug("M4A_Init"), "m4a_init")
        self.assertEqual(source_layout.slug("Registry A8CB"), "registry_a8cb")
        self.assertEqual(source_layout.slug("Actor 086121c4"), "actor_086121C4")

    def test_filename_keeps_exact_symbol_spelling(self):
        self.assertEqual(source_layout.new_filename("Hresvelgr_Create", 0x081A3270), "Hresvelgr_Create.c")
        self.assertEqual(source_layout.new_filename("M4A_Init", 0x08001000), "M4A_Init.c")
        self.assertEqual(source_layout.new_filename("sub_080010a0", 0x080010A0), "sub_080010A0.c")

    def test_explicit_range_and_unknown_address(self):
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            (root / "symbols").mkdir()
            (root / source_layout.SOURCE_DIRS).write_text(
                "current_path,new_path,note\n0822F1B4-0822FDB4,src/lib/m4a,mixer\n")
            self.assertEqual(source_layout.directory_for(0x0822F1B4, root), Path("lib/m4a"))
            with self.assertRaisesRegex(ValueError, r"Example\.c.*source_dirs\.csv.*source_files\.csv"):
                source_layout.new_source(0x0822FDB4, "Example", root)

    def test_player_range_places_new_sources_without_reassigning_an_existing_unit(self):
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            (root / "symbols").mkdir()
            (root / source_layout.SOURCE_DIRS).write_text(
                "current_path,new_path,note\n"
                "0815F5B4-0815F7F4,src/player/target_registry,reviewed range\n"
                "src/player/player_orbit_effect,src/player/effects,existing orbit unit\n")
            (root / source_layout.SOURCE_FILES).write_text(
                "original_path,current_path,rule,filename,start,end\n"
                "src/player_orbit_effect.c,src/player/player_orbit_effect/player_orbit_effect.c,"
                "src/player/player_orbit_effect,orbit_effect.c,0815F7E0,0815FEB0\n")
            self.assertEqual(source_layout.new_source(0x0815F5B4, "Player_FindTarget", root),
                             "src/player/target_registry/Player_FindTarget.c")
            self.assertEqual(source_layout.directory_for(0x0815F7E0, root), Path("player/effects"))

    def test_resolves_names_addresses_aliases_and_drafts(self):
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            (root / "symbols").mkdir()
            (root / "symbols/functions.csv").write_text("addr,name\n08001000,Actor_Move\n")
            directory = root / "src/actors/movement"
            directory.mkdir(parents=True)
            unit = "src/actors/movement/actions.c"
            (root / unit).write_text(
                "#define MoveActor sub_08001000\n"
                "void MoveActor(void) {}\n"
                "#ifdef NONMATCHING\nvoid sub_08001020(void) {}\n#else\n"
                'INCLUDE_ASM("asm/nonmatching", sub_08001020);\n#endif\n'
            )
            for name in ("Actor_Move", "MoveActor", "0x08001000", "sub_08001020", "actions.c"):
                self.assertEqual(source_layout.resolve(name, root), unit)
            self.assertEqual(source_layout.first_function(unit, root), "Actor_Move")
            self.assertEqual(source_layout.first_function(root / unit, root), "Actor_Move")
            manifest = source_layout.scan(root)
            self.assertEqual(manifest.locate("Actor_Move"), (unit, "MoveActor"))
            self.assertEqual(manifest.by_file[unit], [0x08001000, 0x08001020])
            self.assertIsNone(manifest.single_file("08001000"))

    def test_discovers_nested_headers_and_sources(self):
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            paths = [root / "include/engine/testing/record.h", root / "src/actors/testing/sample.c"]
            for path in paths:
                path.parent.mkdir(parents=True, exist_ok=True)
                path.touch()
            self.assertEqual(source_layout.header_paths(root), paths[:1])
            self.assertEqual(source_layout.source_paths(root), paths[1:])

    def test_reuses_manifest_and_invalidates_source_edits_moves_and_symbols(self):
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            for directory in ("build", "src", "symbols"):
                (root / directory).mkdir()
            symbols = root / "symbols/functions.csv"
            symbols.write_text("addr,name\n08001000,Actor_Move\n")
            unit = root / "src/actor.c"
            unit.write_text("void sub_08001000(void) {}\n")
            self.addCleanup(source_layout.manifest.cache_clear)

            self.assertEqual(source_layout.resolve("Actor_Move", root), "src/actor.c")
            source_layout.manifest.cache_clear()
            with patch.object(source_layout, "scan", side_effect=AssertionError("lookup rescanned")):
                self.assertEqual(source_layout.resolve("08001000", root), "src/actor.c")

            unit.write_text("void sub_08001004(void) {}\n")
            source_layout.manifest.cache_clear()
            self.assertIsNone(source_layout.locate("08001000", root))
            self.assertEqual(source_layout.resolve("08001004", root), "src/actor.c")

            symbols.write_text("addr,name\n08001004,Actor_Roll\n")
            source_layout.manifest.cache_clear()
            self.assertEqual(source_layout.resolve("Actor_Roll", root), "src/actor.c")

            unit.rename(root / "src/move.c")
            source_layout.manifest.cache_clear()
            self.assertEqual(source_layout.resolve("Actor_Roll", root), "src/move.c")

            (root / "build/source-layout.json").write_text("{")
            source_layout.manifest.cache_clear()
            self.assertEqual(source_layout.resolve("Actor_Roll", root), "src/move.c")


if __name__ == "__main__":
    unittest.main()
