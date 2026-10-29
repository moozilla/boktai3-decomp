"""Tests of analysis guarantees, not fixtures from already-solved functions."""
import importlib.util
from pathlib import Path
import tempfile
import unittest

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("search_structure", HERE / "search_structure.py")
ss = importlib.util.module_from_spec(spec)
spec.loader.exec_module(ss)


def rows(*instructions):
    return [(i * 2, 2, text, text, None) for i, text in enumerate(instructions)]


class StructureTests(unittest.TestCase):
    def test_alpha_renaming_preserves_real_dependencies(self):
        a = ss.fingerprint(rows("movs r0, #3", "adds r1, r0, #1", "muls r1, r0"))
        b = ss.fingerprint(rows("movs r4, #3", "adds r6, r4, #1", "muls r6, r4"))
        self.assertEqual(a["fingerprint"], b["fingerprint"])

    def test_blanking_registers_would_hide_wrong_dependency(self):
        a = ss.fingerprint(rows("movs r0, #3", "adds r1, r0, #1", "muls r1, r0"))
        b = ss.fingerprint(rows("movs r0, #3", "adds r1, r0, #1", "muls r1, r1"))
        self.assertNotEqual(a["fingerprint"], b["fingerprint"])

    def test_redefinition_does_not_reuse_old_value_id(self):
        a = ss.fingerprint(rows("movs r0, #3", "adds r1, r0, #1", "movs r0, #4", "muls r1, r0"))
        b = ss.fingerprint(rows("movs r0, #3", "adds r1, r0, #1", "movs r2, #4", "muls r1, r0"))
        self.assertNotEqual(a["fingerprint"], b["fingerprint"])

    def test_block_shape_survives_prior_register_change(self):
        a = rows("cmp r0, #0", "beq func+0x8", "movs r0, #1", "b func+0xa", "movs r0, #2", "bx lr")
        b = rows("cmp r0, #0", "beq func+0x8", "movs r4, #1", "b func+0xa", "movs r5, #2", "bx lr")
        result = ss.analyze("int f(int x) { return x; }", "f", a, b)
        self.assertEqual(result["summary"]["regions"], 4)
        self.assertEqual(result["summary"]["local_shape_same"], 4)
        self.assertEqual(result["summary"]["binding_different"], 2)
        self.assertIsNone(result["exact"])

    def test_cfg_join_keeps_both_reaching_definitions(self):
        a = rows("cmp r0, #0", "beq func+0x8", "movs r1, #1", "b func+0xa", "movs r1, #2", "adds r0, r1, #1", "bx lr")
        result = ss.fingerprint(a)
        use = next(x for x in result["webs"]["ambiguous_uses"] if x["offset"] == 10 and x["register"] == "r1")
        self.assertEqual(use["definitions"], ["4:r1", "8:r1"])

    def test_loop_reaching_definition_fixed_point(self):
        result = ss.fingerprint(rows("movs r0, #0", "adds r0, #1", "cmp r0, #4", "blt func+0x2", "bx lr"))
        use = next(x for x in result["webs"]["ambiguous_uses"] if x["offset"] == 2 and x["register"] == "r0")
        self.assertEqual(use["definitions"], ["0:r0", "2:r0"])
        self.assertGreater(result["webs"]["fixed_point_iterations"], 1)

    def test_memory_signedness_and_offset(self):
        result = ss.fingerprint(rows("ldrb r0, [r4, #3]", "ldrsh r1, [r5, r0]", "strh r0, [r4, #6]"))
        self.assertEqual([m["width"] for m in result["memory"]], [1, 2, 2])
        self.assertEqual([m["signed"] for m in result["memory"]], [False, True, None])
        self.assertEqual(result["memory"][0]["displacement"], 3)
        self.assertEqual(result["memory"][1]["index"], ["r0"])

    def test_literal_data_is_not_instructions_or_calls(self):
        result = ss.fingerprint(rows("ldr r0, =0x8123456", "bx lr", ".word 0x12345678"))
        self.assertEqual(result["instruction_count"], 2)
        self.assertEqual(len(result["blocks"]), 1)
        self.assertEqual(result["memory"][0]["literal"], "0x8123456")

    def test_explicit_pool_padding_does_not_create_fake_region(self):
        source = rows("bx lr", "movs r0, r0")
        result = ss.analyze("void f(void) {}", "f", source, source, target_data=[(2, 4)], current_data=[(2, 4)])
        self.assertEqual(result["summary"]["regions"], 1)
        self.assertEqual(result["target"]["instruction_count"], 1)

    def test_region_exposes_concrete_dependency_witness(self):
        a = rows("movs r0, #3", "adds r1, r0, #1", "muls r1, r0")
        b = rows("movs r0, #3", "adds r1, r0, #1", "muls r1, r1")
        result = ss.analyze("void f(void) {}", "f", a, b)
        witness = result["regions"][0]["differences"][0]
        self.assertEqual(witness["kind"], "dependency")
        self.assertEqual(witness["target"]["text"], "muls r1, r0")
        self.assertEqual(witness["current"]["text"], "muls r1, r1")

    def test_calls_are_ordered_and_not_proof(self):
        result = ss.analyze("void f(void) { a(); b(); }", "f", rows("bl #0x8001000", "bl #0x8002000"), rows("bl #0x8002000", "bl #0x8001000"))
        self.assertFalse(result["constraints"]["call_order"]["same"])
        self.assertEqual(result["actions"][0]["category"], "control_flow")
        self.assertIn("hypothesis", result["actions"][0]["confidence"])

    def test_byte_identity_is_independent_of_fingerprints(self):
        r = rows("movs r0, #1")
        result = ss.analyze("int f(void) { return 1; }", "f", r, r, target_bytes=b"a", current_bytes=b"b")
        self.assertFalse(result["exact"])
        self.assertEqual(result["summary"]["local_shape_same"], 1)

    def test_movs_preserves_carry_and_branch_reads_carry_only(self):
        result = ss.fingerprint(rows("cmp r0, #3", "movs r1, #0", "bcs func+0x8", "bx lr", "bx lr"))
        use = next(u for u in result["webs"]["uses"] if u["offset"] == 4 and u["register"] == "flag:C")
        self.assertEqual(use["definitions"], ["0:flag:C"])
        self.assertFalse(any(u["offset"] == 4 and u["register"] in {"flag:N", "flag:Z", "flag:V"} for u in result["webs"]["uses"]))

    def test_fixed_shift_does_not_read_old_carry(self):
        read, written, known = ss._effects(ss._row("lsls r0, r1, #3", 0))
        self.assertNotIn("flag:C", read)
        self.assertIn("flag:C", written)
        self.assertNotIn("flag:V", written)

    def test_adc_reads_carry_and_signed_branch_reads_n_v(self):
        read, written, known = ss._effects(ss._row("adcs r0, r1", 0))
        self.assertIn("flag:C", read)
        self.assertTrue(set(ss.FLAGS) <= set(written))
        read, _, _ = ss._effects(ss._row("bge func+0x10", 0))
        self.assertEqual(read, ["flag:N", "flag:V"])

    def test_unknown_opcode_is_explicit(self):
        result = ss.fingerprint(rows("opaque r0, r1"))
        self.assertEqual(len(result["unknown_instructions"]), 1)
        self.assertFalse(result["blocks"][0]["supported"])

    def test_dynamic_compiler_source_lines(self):
        with tempfile.TemporaryDirectory() as d:
            Path(d, "global.c").write_text("/* line one */\n\nfind_reg (x)\n{}\nallocno_compare (a,b)\n{}\n")
            refs = ss.compiler_references("allocation", d)
        self.assertEqual([(x["symbol"], x["line"]) for x in refs], [("allocno_compare", 5), ("find_reg", 3)])
        self.assertTrue(all(not x["file"].startswith("/") for x in refs))

    def test_source_sites_skip_comments_and_other_functions(self):
        source = 'void other(void) { while(1) {} }\nint f(int x) { /* while(x) */\n if (x) return x;\n return 1;\n}'
        result = ss.analyze(source, "f")
        self.assertTrue(result["source"]["found"])
        self.assertEqual([x["line"] for x in result["source"]["sites"] if x["kind"] == "branch"], [3])
        self.assertFalse(any(x["kind"] == "loop" for x in result["source"]["sites"]))


if __name__ == "__main__":
    unittest.main()
