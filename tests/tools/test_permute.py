"""Regression coverage for comparing linked values and preserving trailing data."""
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
import build
import check
import permute
import split


class PermuteTests(unittest.TestCase):
    def test_compact_nested_body_and_inline_helper_are_preserved(self):
        source = "static inline int\nh(int x) { return x + 1; }\n"
        source += "int f(int n) { if(n>0) { do { n=h(n); } while(n<4); } return n; }\n"
        with tempfile.TemporaryDirectory() as d:
            src = Path(d) / "candidate.c"
            src.write_text(source)
            with patch.object(build, "preprocess", return_value=source):
                self.assertEqual(permute.prepare_source(str(src), "f"), source)
            src.write_text(source + "int other(void) { return 1; }\n")
            with self.assertRaisesRegex(ValueError, "one ROM function"):
                permute.prepare_source(str(src), "f")

    def test_crashed_search_is_not_reported_as_no_improvement(self):
        with tempfile.TemporaryDirectory() as d:
            with self.assertRaisesRegex(RuntimeError, "exit 7"):
                permute.run_search([sys.executable, "-c", "raise SystemExit(7)"], d, 5)

    def test_search_timeout_is_a_normal_budget_stop(self):
        with tempfile.TemporaryDirectory() as d:
            self.assertFalse(permute.run_search(
                [sys.executable, "-c", "import time; time.sleep(60)"], d, 0.1))

    def test_c_extent_includes_literal_pool_but_preserves_binary_tail(self):
        lines = ["fn:\n", "\tbx lr\n", "pool: .4byte 123\n",
                 "tail:\n", "tail_alias:\n", '\t.incbin "original", 4, 8\n']
        self.assertEqual(split.c_replacement_end(lines, 0, len(lines)), 3)
        self.assertEqual(split.c_replacement_end(lines[:3], 0, 3), 3)

    def test_per_file_compiler_flags(self):
        with tempfile.TemporaryDirectory() as d:
            src = Path(d) / "candidate.c"
            src.write_text("// CFLAGS: -O1 -mthumb-interwork\nint f(void) { return 0; }\n")
            self.assertEqual(build.cflags_for(str(src)), ["-O1", "-mthumb-interwork", "-fhex-asm"])

    @unittest.skipUnless(all(shutil.which("arm-none-eabi-" + t) for t in ("as", "ld", "objcopy")),
                         "ARM binutils required for synthetic linked-object fixture")
    def test_numeric_and_symbolic_literals_resolve_identically(self):
        # Synthetic data, independent of the ROM. Include odd Thumb pointers
        # and the project's even-address alias convention.
        with tempfile.TemporaryDirectory() as d:
            linked = []
            symbols = {"fixture": (0x08004001, "FUNC"),
                       "callback": (0x08005001, "FUNC"),
                       "gUnk_02001234": (0x02001234, "NOTYPE")}
            for name, words in (("numeric", "0x02001234, 0x08005001, 0x08005000"),
                                ("symbolic", "gUnk_02001234, callback, callback__addr")):
                src, obj, elf, binary = [str(Path(d) / (name + s)) for s in (".s", ".o", ".elf", ".bin")]
                Path(src).write_text(".syntax unified\n.thumb\n.global fixture\n.thumb_func\n"
                                     "fixture:\n bx lr\n .align 2, 0\n .word " + words + "\n")
                build.sh(build.AS + ["-o", obj, src])
                address, names = check.link_object(obj, elf, syms=symbols)
                self.assertEqual((address, names), (0x08004000, ["fixture"]))
                subprocess.run(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", elf, binary], check=True)
                linked.append(Path(binary).read_bytes())
            self.assertEqual(linked[0], linked[1])
            self.assertEqual(linked[0][-12:], bytes.fromhex("341200020150000800500008"))


if __name__ == "__main__":
    unittest.main()
