"""Coverage uses ROM halfword entry indexes and keeps replay segments separate."""
from contextlib import redirect_stdout
from io import StringIO
import json
from pathlib import Path
import sys
from tempfile import TemporaryDirectory
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
import whoruns


class WhoRunsTest(unittest.TestCase):
    def test_marked_coverage_and_lookup(self):
        base = whoruns.ROM_BASE
        with TemporaryDirectory() as directory:
            index = Path(directory)
            run = index / "one"
            run.mkdir()
            with (run / "cov-idle.exec").open("wb") as stream:
                stream.write(bytes([0, 1, 0, 0]))
                stream.truncate(whoruns.EXEC_SIZE)
            with (run / "cov-action.exec").open("wb") as stream:
                stream.write(bytes([0, 0, 1, 2]))
                stream.truncate(whoruns.EXEC_SIZE)
            whoruns.write_record(run, run, [base + 2, base + 6], "0001", 123, "Example", {"action": 456})
            path = run / "coverage.json"
            self.assertEqual(json.loads(path.read_text())["segments"], {
                "action": ["08000006"], "idle": ["08000002"],
            })
            hits = list(whoruns.sightings(index, {base + 6}, [path]))
            self.assertEqual(hits, [("0001", 456, "action", "Example", Path("one"), ["08000006"])])

    def test_rom_range_is_half_open(self):
        base = whoruns.ROM_BASE
        self.assertEqual(whoruns.resolve("08000002–08000006", [base, base + 2, base + 6]), {base + 2})

    def test_directory_wins_over_same_named_source_file(self):
        files = {"src/ui/book/book.c": [0x08000002], "src/ui/book/pages.c": [0x08000004]}
        with patch.object(whoruns, "manifest") as source:
            source.return_value.names = {}
            source.return_value.by_file = files
            entries = [0x08000002, 0x08000004]
            self.assertEqual(whoruns.resolve("book", entries), set(entries))
            self.assertEqual(whoruns.resolve("book.c", entries), {entries[0]})

    def test_explicit_paths_only_read_their_source_directory(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            unit = root / "src/ui/book"
            unit.mkdir(parents=True)
            path = unit / "book.c"
            path.write_text("void sub_08000002(void) {}\n")
            (unit / "pages.c").write_text("void sub_08000004(void) {}\n")
            entries = [0x08000002, 0x08000004]
            with patch.object(whoruns, "ROOT", root), patch.object(whoruns, "manifest") as all_sources:
                self.assertEqual(whoruns.resolve("src/ui/book", entries), set(entries))
                self.assertEqual(whoruns.resolve("ui/book/book.c", entries), {entries[0]})
                self.assertEqual(whoruns.resolve(str(path), entries), {entries[0]})
                all_sources.assert_not_called()

    def test_multiple_queries_report_each_unit_separately(self):
        with TemporaryDirectory() as directory:
            index = Path(directory)
            record = {"save": "0001", "scene": 123, "label": "Example",
                      "segments": {"idle": ["08000002"], "action": ["08000004"]}}
            (index / "coverage.json").write_text(json.dumps(record))
            stdout = StringIO()
            argv = ["whoruns.py", "--coverage", directory, "08000002", "08000004"]
            with patch.object(sys, "argv", argv), \
                patch.object(whoruns, "rom_entries", return_value=[0x08000002, 0x08000004]), \
                redirect_stdout(stdout):
                whoruns.main()
            idle, action = stdout.getvalue().strip().split("\n\n")
            self.assertTrue(idle.startswith("08000002\n"))
            self.assertIn("\tidle\t", idle)
            self.assertTrue(action.startswith("08000004\n"))
            self.assertIn("\taction\t", action)
