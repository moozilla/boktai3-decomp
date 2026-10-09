"""Reviewed ARM entry points must not swallow neighboring binary code."""
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
import build
import check
import split


class ArmUnitTests(unittest.TestCase):
    @unittest.skipIf(check.capstone is None, "capstone required")
    def test_checker_disassembles_arm_instruction_at_full_width(self):
        self.assertEqual(check.disasm(bytes.fromhex("1eff2fe1"), 0x08004000, arm=True),
                         [(0x08004000, "bx lr")])
        self.assertEqual(check.disasm(bytes.fromhex("7047"), 0x08004000),
                         [(0x08004000, "bx lr")])

    def test_arm_compiler_omits_thumb_only_flag(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "irq.c"
            source.write_text("// COMPILER: agbcc_arm\n// CFLAGS: -O2 -mthumb-interwork\n")
            self.assertEqual(Path(build.compiler_for(str(source))).name, "agbcc_arm")
            self.assertEqual(build.cflags_for(str(source)), ["-O2", "-mthumb-interwork"])

    def expand(self, entries, lines):
        with tempfile.TemporaryDirectory() as directory:
            inventory = Path(directory) / "boundaries.csv"
            inventory.write_text("address,isa\n" + entries)
            return split.apply_extra_boundaries(lines, inventory)

    def test_reviewed_entries_preserve_prefix_and_unmatched_tail(self):
        original = ['prefix:', '\t.incbin "baserom.gba", 0x4000, 0x20', 'suffix:']
        expanded = self.expand("08004004,arm\n08004010,thumb\n", original)
        self.assertEqual(expanded, [
            'prefix:', '\t.incbin "baserom.gba", 0x4000, 0x4',
            '\tarm_func_start sub_08004004', 'sub_08004004: @ 0x08004004',
            '\t.incbin "baserom.gba", 0x4004, 0xc',
            '\tthumb_func_start sub_08004010', 'sub_08004010: @ 0x08004010',
            '\t.incbin "baserom.gba", 0x4010, 0x10', 'suffix:'])
        # Replacing the ARM function consumes only its reviewed range. The
        # separate Thumb entry remains available as an assembly unit.
        self.assertEqual(split.c_replacement_end(expanded, 2, 5, {0x08004004}), 5)
        self.assertEqual(original[1], '\t.incbin "baserom.gba", 0x4000, 0x20')

    def test_malformed_or_unlocated_entries_are_rejected(self):
        blob = ['\t.incbin "baserom.gba", 0x4000, 0x20']
        for entries, message in (
            ("08004002,arm\n", "ISA/alignment"),
            ("08004004,mips\n", "ISA/alignment"),
            ("08004004,arm\n08004004,arm\n", "duplicate"),
            ("08004020,arm\n", "not inside"),
        ):
            with self.subTest(entries=entries), self.assertRaisesRegex(ValueError, message):
                self.expand(entries, blob)


if __name__ == "__main__":
    unittest.main()
