"""proved-zero: the compiler's own bit-field store mask (`and:0`) is not a site; a zero the C proved is."""
import subprocess
import unittest
from unittest import mock

from tools import deadcode

LINES = ("AGBCC_DEAD\tzero\tsrc/x.c\t2\tf\tand:0\n"
         "AGBCC_DEAD\tzero\tsrc/x.c\t3\tf\tashiftrt:8\n"
         "AGBCC_DEAD\tzero\tsrc/x.c\t3\tf\tand:0\n"
         "AGBCC_DEAD\tzero\tsrc/x.c\t4\tf\tand\n")
SOURCE = "void f(void) {\n    flags->bits &= ~2;\n    g->bits &= x >> 8;\n    y = x & 0x300;\n}\n"


class OwnMask(unittest.TestCase):
    def test_own_mask_is_dropped_and_proved_zeros_stay(self):
        done = subprocess.CompletedProcess([], 0, stdout="", stderr=LINES)
        with mock.patch.object(deadcode.subprocess, "run", return_value=done), \
                mock.patch("builtins.open", mock.mock_open(read_data=SOURCE)):
            sites = deadcode.compile_unit("src/x.c")
        self.assertEqual([(line, detail) for _, _, line, _, detail in sites],
                         [(3, "ashiftrt:8"), (4, "and")])


if __name__ == "__main__":
    unittest.main()
