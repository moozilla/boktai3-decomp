"""Coverage uses ROM halfword entry indexes and keeps replay segments separate."""
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
