"""Counterexamples to register-blanking and unsound CFG correspondence.

These synthetic listings exercise the information the search must not erase;
they do not embed game code or pretend solved functions measure search velocity.
"""
from __future__ import annotations

import json
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from matchsearch.constraints import analyze, compare


def listing(*instructions: str) -> str:
    offset, rows = 0, []
    for instruction in instructions:
        rows.append(f"{offset:04X}  {instruction}")
        offset += 4 if instruction.startswith(("bl ", ".word")) else 2
    return "\n".join(rows)


class MachineGraphTests(unittest.TestCase):
    def test_identity_and_json_serialization(self):
        program = analyze(listing("movs r4, #3", "adds r0, r4, #2", "bx lr"))
        result = compare(program, program)
        self.assertTrue(all(n == 0 for n in result["vector"].values()))
        self.assertFalse(result["semantic_proof"])
        json.dumps(program.to_dict())
        json.dumps(result)

    def test_internal_register_renaming_preserves_dependencies(self):
        left = analyze(listing("movs r4, #3", "adds r5, r4, #2", "mov r0, r5", "bx lr"))
        right = analyze(listing("movs r6, #3", "adds r7, r6, #2", "mov r0, r7", "bx lr"))
        result = compare(left, right)
        self.assertEqual(left.fingerprints["dependencies"], right.fingerprints["dependencies"])
        self.assertTrue(all(n == 0 for n in result["vector"].values()))
        self.assertEqual(result["regions"][0]["verdict"], "dependency-compatible")

    def test_same_opcodes_different_dependencies_are_not_compatible(self):
        left = analyze(listing("mov r4, r0", "mov r5, r1", "adds r0, r4, r4", "bx lr"))
        right = analyze(listing("mov r4, r0", "mov r5, r1", "adds r0, r4, r5", "bx lr"))
        result = compare(left, right)
        self.assertEqual(result["vector"]["instructions"], 0)
        self.assertEqual(result["vector"]["operands"], 0)
        self.assertGreater(result["vector"]["dependencies"], 0)
        self.assertNotEqual(left.fingerprints["dependencies"], right.fingerprints["dependencies"])

    def test_scalar_constant_is_not_erased(self):
        left = analyze(listing("movs r4, #3", "mov r0, r4", "bx lr"))
        right = analyze(listing("movs r4, #4", "mov r0, r4", "bx lr"))
        self.assertEqual(compare(left, right)["vector"]["operands"], 1)

    def test_literal_pool_content_is_measured(self):
        left = analyze(listing("bx lr", ".word 0x12345678"))
        right = analyze(listing("bx lr", ".word 0x12345679"))
        self.assertEqual(compare(left, right)["vector"]["data"], 1)

    def test_branch_destinations_are_not_erased(self):
        left = analyze(listing("cmp r0, #0", "beq func+0x8", "movs r0, #1", "bx lr", "movs r0, #2", "bx lr"))
        right = analyze(listing("cmp r0, #0", "beq func+0x4", "movs r0, #1", "bx lr", "movs r0, #2", "bx lr"))
        self.assertGreater(compare(left, right)["vector"]["control"], 0)

    def test_relocated_branch_compares_destination_identity(self):
        # An extra instruction shifts the destination but not the taken edge.
        left = analyze(listing("cmp r0, #0", "beq func+0x8", "movs r0, #1", "bx lr", "movs r0, #2", "bx lr"))
        right = analyze(listing("cmp r0, #0", "beq func+0xA", "movs r6, #99", "movs r0, #1", "bx lr", "movs r0, #2", "bx lr"))
        self.assertEqual(compare(left, right)["vector"]["control"], 0)

    def test_regional_hash_ignores_unrelated_earlier_instruction_numbers(self):
        left = analyze(listing("cmp r0, #0", "beq func+0x8", "movs r0, #1", "bx lr", "movs r0, #2", "bx lr"))
        right = analyze(listing("cmp r0, #0", "beq func+0xA", "movs r6, #99", "movs r0, #1", "bx lr", "movs r0, #2", "bx lr"))
        baseline = compare(left, left)["regions"][-1]
        shifted = compare(left, right)["regions"][-1]
        self.assertIsNotNone(baseline["candidate_dependency_hash"])
        self.assertEqual(baseline["candidate_dependency_hash"], shifted["candidate_dependency_hash"])

    def test_call_target_is_a_constraint(self):
        left = analyze(listing("movs r0, #1", "bl CalleeA", "bx lr"))
        right = analyze(listing("movs r0, #1", "bl CalleeB", "bx lr"))
        result = compare(left, right)
        self.assertEqual(result["vector"]["calls"], 1)
        self.assertEqual(result["vector"]["operands"], 1)

    def test_call_arguments_remain_abi_bound(self):
        left = analyze(listing("movs r0, #1", "movs r1, #2", "bl Callee", "bx lr"))
        right = analyze(listing("movs r1, #1", "movs r0, #2", "bl Callee", "bx lr"))
        result = compare(left, right)
        self.assertGreater(result["vector"]["dependencies"], 0)
        self.assertTrue(any(i.get("role") in ("arg0", "arg1") for i in result["issues"]))

    def test_known_call_signature_does_not_invent_unused_arguments(self):
        source = listing("movs r0, #1", "bl Callee", "bx lr")
        result = analyze(source, call_arities={"Callee": 1})
        call = next(i for i in result.instructions if i.callee == "Callee")
        self.assertIn("arg0", result.dependencies[call.index])
        self.assertNotIn("arg1", result.dependencies[call.index])

    def test_memory_signedness_and_width_are_not_erased(self):
        unsigned = analyze(listing("ldrh r0, [r1]", "bx lr"))
        signed = analyze(listing("ldrsh r0, [r1, r2]", "bx lr"))
        byte = analyze(listing("ldrb r0, [r1]", "bx lr"))
        self.assertEqual(compare(unsigned, signed)["vector"]["memory"], 1)
        self.assertEqual(compare(unsigned, byte)["vector"]["memory"], 1)

    def test_address_base_and_store_value_have_distinct_roles(self):
        left = analyze(listing("str r0, [r1]", "bx lr"))
        right = analyze(listing("str r1, [r0]", "bx lr"))
        result = compare(left, right)
        self.assertEqual(result["vector"]["instructions"], 0)
        self.assertGreaterEqual(result["vector"]["dependencies"], 2)

    def test_copy_and_add_zero_preserve_data_provenance(self):
        result = analyze(listing("mov r4, r0", "adds r5, r4, #0", "str r5, [r1]", "bx lr"))
        self.assertEqual(result.dependencies[2]["value"], ("entry:r0",))

    def test_overwrite_kills_prior_definition(self):
        result = analyze(listing("movs r4, #1", "movs r4, #2", "mov r0, r4", "bx lr"))
        self.assertEqual(result.dependencies[2]["input0"], ("n1:value",))

    def test_phi_alternatives_come_from_cfg_not_listing_order(self):
        result = analyze(listing("cmp r0, #0", "beq func+0x8", "movs r4, #1", "b func+0xA",
                                 "movs r4, #2", "mov r0, r4", "bx lr"))
        self.assertEqual(set(result.dependencies[5]["input0"]), {"n2:value", "n4:value"})

    def test_loop_reaches_a_finite_fixpoint(self):
        result = analyze(listing("movs r4, #0", "adds r4, #1", "cmp r4, #3", "blt func+0x2",
                                 "mov r0, r4", "bx lr"))
        self.assertEqual(set(result.dependencies[1]["input0"]), {"n0:value", "n1:value"})
        self.assertTrue(result.back_edges)

    def test_carry_survives_movs_and_is_read_by_adc(self):
        result = analyze(listing("cmp r0, #1", "movs r1, #0", "adcs r1, r2", "bx lr"))
        self.assertEqual(result.dependencies[2]["carry"], ("n0:flag.C",))

    def test_branch_reads_individual_flag_definitions(self):
        result = analyze(listing("cmp r0, #1", "movs r1, #0", "bhi func+0x8", "bx lr", "bx lr"))
        self.assertEqual(result.dependencies[2]["condition.C"], ("n0:flag.C",))
        self.assertEqual(result.dependencies[2]["condition.Z"], ("n1:flag.Z",))

    def test_unknown_instruction_is_explicit(self):
        result = analyze(listing("mystery r0, r1", "bx lr"))
        self.assertTrue(any("Unsupported opcode" in w for w in result.warnings))
        self.assertFalse(compare(result, result)["regions"][0]["register_neutral"])

    def test_scope_rows_retain_local_targets(self):
        rows = [(0, 2, "cmp r0, #0", "cmp r0, #0", None),
                (2, 2, "beq func+0x6", "beq ->", 6),
                (4, 2, "movs r0, #1", "movs r0, #1", None),
                (6, 2, "bx lr", "bx lr", None)]
        result = analyze(rows, address=0x08000100)
        self.assertEqual(result.instructions[1].target, 6)
        self.assertTrue(all(n == 0 for n in compare(result, result)["vector"].values()))

    def test_switch_table_preserves_case_order(self):
        source = """Switch @ 08000100
          0 lsls r0, r0, #2
          2 ldr r1, =func+0xC
          4 adds r0, r0, r1
          6 ldr r0, [r0]
          8 mov pc, r0
          A movs r0, r0
          C .word 0x08000114
          10 .word 0x08000118
          14 movs r0, #1
          16 bx lr
          18 movs r0, #2
          1A bx lr
        """
        left = analyze(source)
        right = analyze(source.replace("C .word 0x08000114", "C .word 0x08000118"))
        self.assertEqual(left.jump_tables, {4: (0x14, 0x18)})
        self.assertTrue(all(b.reachable for b in left.blocks if b.start in (0x14, 0x18)))
        self.assertGreater(compare(left, right)["vector"]["control"], 0)

    def test_local_code_pointer_is_not_erased(self):
        left = analyze(listing("ldr r0, =func+0x4", "bx lr", "bx lr", "bx lr"))
        right = analyze(listing("ldr r0, =func+0x6", "bx lr", "bx lr", "bx lr"))
        self.assertGreater(compare(left, right)["vector"]["control"], 0)

    def test_affine_address_recovers_field_and_signed_width(self):
        program = analyze(listing("movs r2, #0xd3", "lsls r2, r2, #2", "adds r4, r0, r2",
                                  "movs r3, #0", "ldrsh r1, [r4, r3]", "bx lr"))
        load = program.resolved_addresses[0]
        self.assertEqual(load["terms"], [{"value": "entry:r0", "coefficient": 1}])
        self.assertEqual(load["displacement"], 0x34C)
        self.assertEqual(load["width"], 16)
        self.assertTrue(load["signed"])

    def test_affine_address_tracks_stack_adjustments(self):
        program = analyze(listing("push {r4, lr}", "sub sp, #16", "str r0, [sp, #4]", "bx lr"))
        store = program.resolved_addresses[1]
        self.assertEqual(store["terms"], [{"value": "entry:sp", "coefficient": 1}])
        self.assertEqual(store["displacement"], -20)

    def test_affine_address_retains_ambiguous_loop_origin(self):
        program = analyze(listing("mov r4, r0", "ldr r1, [r4]", "adds r4, #4", "cmp r1, #0",
                                  "bne func+0x2", "bx lr"))
        load = program.resolved_addresses[0]
        self.assertTrue(load["has_phi"])
        self.assertIsNone(load["absolute"])

    def test_affine_constants_wrap_at_32_bits(self):
        program = analyze(listing("ldr r2, =0xFFFFFFFF", "adds r2, #2", "ldrb r0, [r2]", "bx lr"))
        self.assertEqual(program.resolved_addresses[0]["absolute"], 1)

    def test_signed_memory_value_is_not_assumed_positive(self):
        program = analyze(listing("ldrsh r2, [r0, r1]", "adds r2, #8", "ldrb r3, [r2]", "bx lr"))
        fact = program.resolved_addresses[1]
        self.assertIsNone(fact["absolute"])
        self.assertEqual(fact["terms"], [{"value": "n0:value", "coefficient": 1}])
        self.assertEqual(fact["displacement"], 8)

    def test_pc_reads_use_instruction_address_not_an_entry_register(self):
        program = analyze(listing("mov r4, pc", "mov r5, pc", "ldr r0, [r5]", "bx lr"), address=0x08000100)
        self.assertNotEqual(program.dependencies[0]["input0"], program.dependencies[1]["input0"])
        self.assertEqual(program.resolved_addresses[0]["absolute"], 0x08000106)


if __name__ == "__main__":
    unittest.main()
