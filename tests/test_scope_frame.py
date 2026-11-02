"""Frame correspondence must not follow a repeated instruction's wrong alignment."""
import contextlib
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from tools.matchsearch.evaluator import _load_scope


class FrameCorrespondence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def rows(self, rom, draft):
        decoded = lambda texts: [(i * 2, 2, text, text, None) for i, text in enumerate(texts)]
        pairs, _ = self.scope.align(decoded(rom), decoded(draft))
        return [{"rom_addr": f"{0x08001000 + x * 2:08X}" if x is not None else None,
                 "rom": rom[x] if x is not None else None,
                 "draft_off": y * 2 if y is not None else None,
                 "draft": draft[y] if y is not None else None}
                for x, y, _ in pairs]

    def repeated_accesses(self):
        rom = ["sub sp, #0x10",
               "str r0, [sp, #4]", "str r0, [sp, #8]",
               "ldr r0, [sp, #4]", "ldr r0, [sp, #8]",
               "str r0, [sp, #4]", "str r0, [sp, #8]",
               "ldr r0, [sp, #4]", "ldr r0, [sp, #8]",
               "add sp, #0x10", "bx lr"]
        draft = [text.replace("#4]", "#TEMP]").replace("#8]", "#4]").replace("#TEMP]", "#8]")
                 for text in rom]
        return rom, draft

    def run_frame(self, rows):
        with tempfile.TemporaryDirectory() as directory:
            Path(directory, "diff.json").write_text(json.dumps(rows))
            output = io.StringIO()
            with patch.object(self.scope, "score", return_value=(4, {"wd": directory})), \
                    patch.object(self.scope, "frame_slots", return_value={
                        4: (101, 4, 1, -1), 8: (102, 4, 2, -1)}), \
                    patch.object(self.scope, "pseudo_names", return_value={101: "first", 102: "second"}), \
                    contextlib.redirect_stdout(output):
                self.assertEqual(self.scope.cmd_frame("synthetic.c", "Target", {}), 1)
            return output.getvalue()

    def test_aligned_vote_tie_favors_the_same_access_register(self):
        rom = ["str r4, [sp, #4]", "ldr r2, [sp, #4]", "ldr r0, [sp, #8]"]
        draft = ["str r5, [sp, #4]", "ldr r2, [sp, #8]", "ldr r0, [sp, #4]"]
        rows = [{"rom_addr": f"{0x08001000 + 2 * i:08X}", "draft_off": 2 * i,
                 "rom": a, "draft": b} for i, (a, b) in enumerate(zip(rom, draft))]
        pair, physical, _ = self.scope.frame_slot_pairs(rows)
        self.assertIsNone(physical)
        self.assertEqual(pair, {4: 8, 8: 4})

    def test_repeated_loads_and_stores_use_physical_offsets(self):
        rows = self.rows(*self.repeated_accesses())
        self.assertTrue(any(row["rom"] and row["draft"]
                            and int(row["rom_addr"], 16) - 0x08001000 != row["draft_off"]
                            for row in rows))
        self.assertEqual(self.scope.physical_frame_slot_pairs(rows), {4: 8, 8: 4})
        output = self.run_frame(rows)
        self.assertIn("Slot correspondence uses identical instruction offsets", output)
        self.assertRegex(output, r"sp\+0x4\s+sp\+0x8\s+102\s+second")
        self.assertRegex(output, r"sp\+0x8\s+sp\+0x4\s+101\s+first")
        self.assertIn("Physical slot correspondence gives this pseudo order", output)
        self.assertNotIn("Aligned slot uses suggest", output)

    def test_non_sp_register_difference_keeps_aligned_fallback(self):
        rom, draft = self.repeated_accesses()
        draft[3] = draft[3].replace("r0", "r1")
        rows = self.rows(rom, draft)
        self.assertIsNone(self.scope.physical_frame_slot_pairs(rows))
        self.assertNotIn("Slot correspondence uses identical instruction offsets", self.run_frame(rows))

    def test_other_instruction_differences_reject_physical_path(self):
        for rom, draft in (("b func+0x20", "b func+0x22"),
                           ("adds r0, r1, #4", "adds r0, r1, #8"),
                           ("ldr r0, =0x1234", "ldr r0, =0x5678"),
                           ("str r0, [sp, #4]", "ldr r0, [sp, #8]")):
            with self.subTest(rom=rom, draft=draft):
                self.assertIsNone(self.scope.physical_frame_slot_pairs(self.rows([rom], [draft])))

    def test_one_rom_slot_cannot_map_to_two_draft_slots(self):
        rows = self.rows(["str r0, [sp, #4]", "ldr r0, [sp, #4]"],
                         ["str r0, [sp, #8]", "ldr r0, [sp, #12]"])
        self.assertIsNone(self.scope.physical_frame_slot_pairs(rows))

    def test_two_rom_slots_cannot_map_to_one_draft_slot(self):
        rows = self.rows(["str r0, [sp, #4]", "ldr r0, [sp, #8]"],
                         ["str r0, [sp, #12]", "ldr r0, [sp, #12]"])
        self.assertIsNone(self.scope.physical_frame_slot_pairs(rows))

    def test_complete_unique_offset_sets_are_required(self):
        for change in ("missing", "shifted", "duplicate", "missing_address"):
            with self.subTest(change=change):
                rows = self.rows(*self.repeated_accesses())
                if change == "missing":
                    rows[-1]["draft"] = rows[-1]["draft_off"] = None
                elif change == "shifted":
                    rows[-1]["draft_off"] += 2
                elif change == "duplicate":
                    rows.append(dict(rows[-1]))
                else:
                    rows[-1]["draft_off"] = None
                self.assertIsNone(self.scope.physical_frame_slot_pairs(rows))

    def test_implicit_zero_and_hex_displacements(self):
        rows = self.rows(["str r0, [sp]", "ldr r0, [sp, #0]"],
                         ["str r0, [sp, #0x8]", "ldr r0, [sp, #8]"])
        self.assertEqual(self.scope.physical_frame_slot_pairs(rows), {0: 8})

    def test_empty_listings_do_not_claim_physical_correspondence(self):
        self.assertIsNone(self.scope.physical_frame_slot_pairs([]))


if __name__ == "__main__":
    unittest.main()
