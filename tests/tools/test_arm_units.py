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

    def test_cuts_take_the_mode_of_their_block(self):
        original = ['\tarm_func_start prefix', 'prefix: @ 0x08004000', '\t.incbin "baserom.gba", 0x4000, 0x10',
                    '\tthumb_func_start other', 'other: @ 0x08004010', '\t.incbin "baserom.gba", 0x4010, 0x10']
        expanded = split.apply_extra_boundaries(original, {0x08004004, 0x08004012})
        self.assertEqual(expanded, [
            '\tarm_func_start prefix', 'prefix: @ 0x08004000', '\t.incbin "baserom.gba", 0x4000, 0x4',
            '\tarm_func_start sub_08004004', 'sub_08004004: @ 0x08004004',
            '\t.incbin "baserom.gba", 0x4004, 0xc',
            '\tthumb_func_start other', 'other: @ 0x08004010', '\t.incbin "baserom.gba", 0x4010, 0x2',
            '\tthumb_func_start sub_08004012', 'sub_08004012: @ 0x08004012',
            '\t.incbin "baserom.gba", 0x4012, 0xe'])
        # Replacing the ARM function consumes only its own block. The
        # Thumb function after it remains available as an assembly unit.
        self.assertEqual(split.c_replacement_end(expanded, 3, 6, {0x08004004}), 6)
        self.assertEqual(original[2], '\t.incbin "baserom.gba", 0x4000, 0x10')

    def test_misaligned_or_unlocated_starts_are_rejected(self):
        blob = ['\tarm_func_start prefix', 'prefix: @ 0x08004000', '\t.incbin "baserom.gba", 0x4000, 0x20']
        for starts, message in (({0x08004002}, "misaligned"), ({0x08004020}, "outside a raw block")):
            with self.subTest(starts=starts), self.assertRaisesRegex(ValueError, message):
                split.apply_extra_boundaries(blob, starts)

    def test_reviewed_body_fragments_stop_at_exact_endpoint(self):
        lines = ['sub_08004000:',
                 '\t.incbin "baserom.gba", 0x4000, 0x8',
                 '\t.4byte callback',
                 '\t.incbin "baserom.gba", 0x400c, 0x4',
                 '\t.incbin "baserom.gba", 0x4010, 0x8']
        self.assertEqual(split.c_replacement_end(lines, 0, len(lines),
                         {0x08004000}, [(0x08004000, 0x08004010)]), 4)
        # A fragment extending past the reviewed endpoint stays assembly.
        lines[3] = '\t.incbin "baserom.gba", 0x400c, 0x8'
        self.assertEqual(split.c_replacement_end(lines, 0, len(lines),
                         {0x08004000}, [(0x08004000, 0x08004010)]), 3)


if __name__ == "__main__":
    unittest.main()
