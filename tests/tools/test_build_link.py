import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
import build


class LinkTests(unittest.TestCase):
    def test_symbol_readers_only_see_completed_links(self):
        with tempfile.TemporaryDirectory() as root:
            output = Path(root, "boktai3.elf")
            for fail in (False, True):
                with self.subTest(fail=fail):
                    output.write_bytes(b"old")

                    def linker(command):
                        staging = Path(command[-1])
                        staging.write_bytes(b"partial")
                        self.assertEqual(output.read_bytes(), b"old")
                        if fail:
                            raise SystemExit(1)
                        staging.write_bytes(b"new")

                    with patch.object(build, "ROOT", root), patch.object(build, "sh", linker):
                        if fail:
                            with self.assertRaises(SystemExit):
                                build.link_elf(output.name)
                        else:
                            build.link_elf(output.name)
                    self.assertEqual(output.read_bytes(), b"old" if fail else b"new")
                    self.assertEqual(list(Path(root).iterdir()), [output])
