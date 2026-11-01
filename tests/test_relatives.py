import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import relatives


class NamedAssemblyLabels(unittest.TestCase):
    def test_address_and_name_find_the_same_body(self):
        with tempfile.TemporaryDirectory() as root:
            gen = Path(root) / "gen"
            gen.mkdir()
            (gen / "code.s").write_text(
                "\tthumb_func_start Actor_Update\nActor_Update: @ 0x08001000\n\tmovs r0, #1\n"
                "\tthumb_func_start Interior\nInterior: @ 0x08001002\n\tbx lr\n"
            )
            with (
                patch.object(relatives, "ROOT", root),
                patch.object(relatives, "_funcs", None),
                patch.object(relatives, "names", return_value={"sub_08001000": "Actor_Update"}),
                patch("split.extra_function_starts", return_value=set()),
                patch("split.function_nonentries", return_value={0x08001002}),
            ):
                for arg in ("Actor_Update", "08001000", "0x08001000", "sub_08001000"):
                    self.assertEqual(relatives.resolve(arg), "sub_08001000")
                entries = relatives.functions()
                self.assertEqual(list(entries), ["sub_08001000"])
                self.assertEqual(entries["sub_08001000"][:2], [0x08001000, ["movs r, #1", "bx"]])
