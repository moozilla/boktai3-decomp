from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import header_dependents


class HeaderDependentsTests(unittest.TestCase):
    def test_check_supplies_stock_build_cache_to_the_private_comparison(self):
        with tempfile.TemporaryDirectory() as scratch:
            source = "src/scene/actor/test.c"
            root = Path(scratch)
            cache = root / "build/src/scene/actor"
            cache.mkdir(parents=True)
            (cache / "test.o").write_bytes(b"cached object")
            (cache / "test.o.hash").write_text("stock build stamp")
            workspace = root / "checks"
            workspace.mkdir()

            def linker(path, work, **kwargs):
                self.assertEqual(path, source)
                self.assertEqual((Path(work) / "test.o").read_bytes(), b"cached object")
                self.assertEqual((Path(work) / "test.o.hash").read_text(), "stock build stamp")
                return "", 0x08000000, [], b"exact"

            with (patch.object(header_dependents, "ROOT", root),
                  patch.object(header_dependents.unit_check, "linked_bytes", side_effect=linker)):
                self.assertEqual(header_dependents.check(source, workspace, {}, {}, b"exact"),
                                 (source, True, ""))
            self.assertEqual(list(workspace.iterdir()), [])
