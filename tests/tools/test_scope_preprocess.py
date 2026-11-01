import importlib.util
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools" / "scope"))


class PreprocessTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc") and importlib.util.find_spec("capstone"),
                         "requires the scope environment and a C preprocessor")
    def test_same_include_resolves_beside_each_original_source(self):
        import scope

        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)

            def preprocess(command):
                return subprocess.check_output(command, cwd=root, text=True)

            with patch.object(scope, "ROOT", directory), patch.object(scope, "WORK", directory), \
                 patch.object(scope, "_HEADS", {}), patch.object(scope.build, "sh", preprocess), \
                 patch.object(scope, "cpp_command", lambda path: ["cc", "-E", path]):
                for name in ("first", "second"):
                    source = root / name / "draft.c"
                    source.parent.mkdir()
                    source.write_text('#include "local.h"\nint value;\n')
                    source.with_name("local.h").write_text(f"int {name};\n")
                    self.assertIn(f"int {name};", scope.fast_preprocess(str(source)))
            self.assertEqual(list((root / "cache").iterdir()), [])
