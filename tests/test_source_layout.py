import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import source_layout


class SourceLayoutTests(unittest.TestCase):
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
