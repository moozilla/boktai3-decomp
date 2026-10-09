import tempfile
import unittest
from pathlib import Path

from tools.function_context import CODE_END, DATA_GAPS, Function, ROM_BASE, ROM_SIZE, build_report, parse_functions


class FunctionContextTests(unittest.TestCase):
    def test_parses_all_start_modes_and_label_orders(self):
        with tempfile.TemporaryDirectory() as td:
            asm = Path(td) / "code_sym.s"
            asm.write_text("""thumb_func_start sub_08000000
sub_08000000: @ 0x08000000
 nop
sub_08000002: @ 0x08000002
non_word_aligned_thumb_func_start sub_08000002
 nop
arm_func_start AgbMain
AgbMain: @ 0x0800145C
 nop
""")
            functions = parse_functions(asm)
        self.assertEqual([(f.address, f.name, f.mode, f.end) for f in functions], [
            (ROM_BASE, "sub_08000000", "thumb", ROM_BASE + 2),
            (ROM_BASE + 2, "sub_08000002", "thumb", DATA_GAPS[0][0]),
            (0x0800145C, "AgbMain", "arm", DATA_GAPS[1][0]),
        ])

    def test_exclusive_ranking_and_entry_vs_interior(self):
        funcs = [Function(ROM_BASE, "menu", "thumb", ROM_BASE + 4),
                 Function(ROM_BASE + 4, "engine", "arm", ROM_BASE + 8)]
        menu = bytearray(ROM_SIZE // 2)
        field = bytearray(ROM_SIZE // 2)
        menu[0] = 1                 # entry observed
        menu[1] = 1                 # another halfword in menu
        menu[3] = 2                 # ARM engine reached from menu
        field[3] = 2                # common engine entry
        menu[4] = 4                 # reserved bit must not count as execution
        report = build_report(funcs, {"menu": menu, "field": field}, ["menu"], ["field"])
        rows = {r["name"]: r for r in report["functions"]}
        self.assertTrue(rows["menu"]["exclusive_to_selected_vs_baseline"])
        self.assertTrue(rows["menu"]["selected_entry_observed"])
        self.assertFalse(rows["engine"]["exclusive_to_selected_vs_baseline"])
        self.assertFalse(rows["engine"]["selected_entry_observed"])
        self.assertTrue(rows["engine"]["selected_any_halfword_observed"])
        self.assertEqual(rows["menu"]["mode"], "thumb")
        self.assertEqual(report["coverage_diagnostics"]["menu"]["unattributed_mode_halfwords"], 0)

    def test_empty_assembly_and_invalid_bounds(self):
        with tempfile.TemporaryDirectory() as td:
            asm = Path(td) / "empty.s"
            asm.write_text("")
            self.assertEqual(parse_functions(asm), [])
        with self.assertRaises(ValueError):
            Function(ROM_BASE + 4, "reversed", "thumb", ROM_BASE)
        with self.assertRaises(ValueError):
            Function(ROM_BASE, "outside", "thumb", CODE_END + 2)

    def test_overlay_hits_are_unattributed_and_spans_are_clipped(self):
        funcs = [Function(DATA_GAPS[0][0] - 2, "before_overlay", "thumb", DATA_GAPS[0][0])]
        cov = bytearray(ROM_SIZE // 2)
        cov[(DATA_GAPS[0][0] - ROM_BASE) // 2] = 1
        report = build_report(funcs, {"scene": cov}, ["scene"], [])
        self.assertEqual(report["coverage_diagnostics"]["scene"]["unattributed_mode_halfwords"], 1)


if __name__ == "__main__":
    unittest.main()
