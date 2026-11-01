import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools" / "scope"))
import fast


class AssembleTests(unittest.TestCase):
    def test_overlapping_calls_keep_separate_objects(self):
        with tempfile.TemporaryDirectory() as workdir:
            def assemble(command, **kwargs):
                output, source = map(Path, command[-2:])
                marker = source.read_text().split()[0]
                output.write_text(marker)
                if marker == "outer":
                    self.assertEqual(fast.assemble(["as"], "inner", workdir, "verify"),
                                     (b"inner", ""))
                return subprocess.CompletedProcess(command, 0)

            with patch.object(fast.subprocess, "run", side_effect=assemble):
                self.assertEqual(fast.assemble(["as"], "outer", workdir, "verify"),
                                 (b"outer", ""))
            self.assertEqual(list(Path(workdir).iterdir()), [])
