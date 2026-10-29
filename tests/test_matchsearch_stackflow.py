"""Concrete machine counterexamples for stack transport and internal long jumps."""

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


class InternalLongBranches(unittest.TestCase):
    def test_local_bl_preserves_registers_flags_memory_and_only_changes_link(self):
        graph = analyze(listing("movs r0, #7", "cmp r1, #2", "bl func+0xA", "movs r0, #99",
                                "mov r2, r0", "mov r3, lr", "bx lr"), address=0x8000000)
        jump = graph.instructions[2]
        self.assertEqual(jump.control, "branch")
        self.assertIsNone(jump.callee)
        self.assertEqual([(item.role, item.register) for item in jump.definitions], [("link", "lr")])
        self.assertEqual(graph.dependencies[4]["input0"], ("n0:value",))
        self.assertEqual(graph.dependencies[5]["input0"], ("n2:link",))
        self.assertFalse(graph.blocks[1].reachable)
        self.assertEqual(graph.blocks[0].successors, [("taken", 2)])

    def test_local_bl_target_is_compared(self):
        a = analyze(listing("bl func+0x6", "bx lr", "movs r0, #1", "bx lr"))
        b = analyze(listing("bl func+0x4", "bx lr", "movs r0, #1", "bx lr"))
        self.assertGreater(compare(a, b)["vector"]["control"], 0)
        self.assertEqual(compare(a, b)["vector"]["calls"], 0)

    def test_numeric_call_only_becomes_local_at_a_decoded_instruction(self):
        graph = analyze(listing("bl #0x8000008", "bl #0x8100000", "mov r2, r0", "bx lr"), address=0x8000000)
        self.assertEqual(graph.instructions[0].control, "branch")
        self.assertEqual(graph.instructions[1].control, "call")
        self.assertEqual(graph.instructions[1].callee, "#0x8100000")

    def test_local_bl_link_value_is_the_thumb_return_address(self):
        graph = analyze(listing("bl func+0x6", "bx lr", "mov r1, lr", "subs r1, #1", "ldrh r0, [r1]", "bx lr"),
                        address=0x8000000)
        load = next(fact for fact in graph.resolved_addresses if fact["offset"] == 10)
        self.assertEqual(load["absolute"], 0x8000004)

    def test_external_call_clobbers_thumb_fixed_call_used_r11(self):
        graph = analyze(listing("mov r4, fp", "bl Callee", "mov r0, fp", "mov r1, r4", "bx lr"))
        self.assertEqual(graph.dependencies[2]["input0"], ("n1:call.r11",))
        self.assertEqual(graph.dependencies[3]["input0"], ("entry:r11",))

    def test_register_lists_use_architectural_order_and_expand_ranges(self):
        graph = analyze(listing("push {lr, r4-r6}", "pop {r4-r6, pc}"))
        self.assertEqual(graph.instructions[0].memory.count, 4)
        self.assertEqual([(item.role, item.register) for item in graph.instructions[0].uses
                          if item.role.startswith("value")],
                         [("value0", "r4"), ("value1", "r5"), ("value2", "r6"), ("value3", "lr")])


class StackTransport(unittest.TestCase):
    def graph(self, *instructions, **kwargs):
        graph = analyze(listing(*instructions), **kwargs)
        json.dumps(graph.to_dict())
        self.assertFalse(graph.stack_transport["semantic_proof"])
        self.assertFalse(graph.stack_transport["stats"].get("budget_exhausted"))
        return graph

    def aliases(self, graph):
        return graph.stack_transport["value_aliases"]

    def test_word_spill_preserves_physical_graph_and_adds_identity_view(self):
        graph = self.graph("sub sp, #8", "str r0, [sp, #4]", "ldr r1, [sp, #4]", "adds r2, r1, #1", "bx lr")
        alias = self.aliases(graph)["n2:value"]
        self.assertEqual(alias["values"], ["entry:r0"])
        self.assertEqual(alias["stack_offset"], -4)
        self.assertEqual(alias["stores"], [2])
        self.assertEqual(graph.dependencies[3]["input0"], ("n2:value",))
        self.assertEqual(graph.transport_dependencies[3]["input0"], ("entry:r0",))
        self.assertEqual(graph.dependencies[2]["memory"], graph.transport_dependencies[2]["memory"])

    def test_push_pop_architectural_order_and_high_register_copies(self):
        graph = self.graph("mov r4, r8", "push {lr, r4}", "movs r4, #0", "pop {r2, r3}", "mov r8, r2", "bx r3")
        self.assertEqual(self.aliases(graph)["n3:value0"]["values"], ["entry:r8"])
        self.assertEqual(self.aliases(graph)["n3:value1"]["values"], ["entry:lr"])
        self.assertEqual(graph.transport_dependencies[4]["input0"], ("entry:r8",))
        self.assertEqual(graph.stack_transport["block_sp"][0]["exit"], 0)

    def test_known_stack_alias_and_index_use_same_slot(self):
        graph = self.graph("sub sp, #16", "mov r4, sp", "movs r2, #8", "str r0, [r4, r2]", "ldr r1, [sp, #8]", "bx lr")
        self.assertEqual(self.aliases(graph)["n4:value"]["values"], ["entry:r0"])
        self.assertEqual(self.aliases(graph)["n4:value"]["stack_offset"], -8)

    def test_unknown_indexed_store_invalidates(self):
        graph = self.graph("sub sp, #8", "str r0, [sp]", "mov r4, sp", "str r1, [r4, r2]", "ldr r3, [sp]", "bx lr")
        self.assertNotIn("n4:value", self.aliases(graph))

    def test_unknown_nonstack_store_is_not_assumed_disjoint_in_strict_mode(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "str r2, [r1]", "ldr r3, [sp]", "bx lr")
        self.assertNotIn("n3:value", self.aliases(graph))
        self.assertTrue(any("may alias" in fact.get("reason", "") for fact in graph.stack_transport["facts"]))

    def test_partial_overlap_kills_word_but_preserves_disjoint_neighbor(self):
        graph = self.graph("sub sp, #8", "mov r4, sp", "str r0, [sp]", "str r1, [sp, #4]", "strb r2, [r4, #1]",
                           "ldr r3, [sp]", "ldr r4, [sp, #4]", "bx lr")
        self.assertNotIn("n5:value", self.aliases(graph))
        self.assertEqual(self.aliases(graph)["n6:value"]["values"], ["entry:r1"])

    def test_narrow_load_records_bit_extraction_without_false_identity(self):
        graph = self.graph("sub sp, #4", "mov r4, sp", "movs r5, #0", "str r0, [sp]",
                           "ldrh r1, [r4, #2]", "ldrsb r2, [r4, r5]", "bx lr")
        facts = {fact.get("definition"): fact for fact in graph.stack_transport["facts"] if fact["kind"] == "load"}
        self.assertEqual(facts["n4:value"]["status"], "typed-bits")
        self.assertEqual(facts["n4:value"]["bit_offset"], 16)
        self.assertEqual(facts["n4:value"]["extension"], "zero")
        self.assertEqual(facts["n5:value"]["extension"], "sign")
        self.assertEqual(self.aliases(graph), {})

    def test_narrow_constant_identity_requires_correct_sign_extension(self):
        graph = self.graph("sub sp, #4", "mov r4, sp", "movs r5, #0", "movs r0, #0x80",
                           "strb r0, [r4]", "ldrb r1, [r4]", "ldrsb r2, [r4, r5]", "bx lr")
        self.assertIn("n5:value", self.aliases(graph))
        self.assertNotIn("n6:value", self.aliases(graph))

    def test_signed_loaded_value_can_survive_same_signed_width(self):
        graph = self.graph("sub sp, #4", "mov r4, sp", "movs r5, #0", "ldrsh r0, [r1, r2]",
                           "strh r0, [r4]", "ldrsh r3, [r4, r5]", "ldrh r6, [r4]", "bx lr")
        self.assertEqual(self.aliases(graph)["n5:value"]["values"], ["n3:value"])
        self.assertNotIn("n6:value", self.aliases(graph))

    def test_adjacent_halfword_stores_do_not_claim_a_full_word_identity(self):
        graph = self.graph("sub sp, #4", "mov r4, sp", "strh r0, [r4]", "strh r1, [r4, #2]", "ldr r2, [sp]", "bx lr")
        self.assertNotIn("n4:value", self.aliases(graph))

    def test_join_retains_both_all_path_source_values_and_stores(self):
        graph = self.graph("sub sp, #4", "cmp r2, #0", "beq func+0xA", "str r0, [sp]", "b func+0xC",
                           "str r1, [sp]", "ldr r3, [sp]", "bx lr")
        alias = self.aliases(graph)["n6:value"]
        self.assertEqual(alias["values"], ["entry:r0", "entry:r1"])
        self.assertEqual(alias["stores"], [6, 10])

    def test_join_missing_store_is_unknown(self):
        graph = self.graph("sub sp, #4", "cmp r2, #0", "beq func+0x8", "str r0, [sp]", "ldr r3, [sp]", "bx lr")
        self.assertNotIn("n4:value", self.aliases(graph))

    def test_loop_preserves_preheader_store(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "movs r3, #3", "ldr r1, [sp]",
                           "subs r3, #1", "bne func+0x6", "add sp, #4", "bx lr")
        self.assertEqual(self.aliases(graph)["n3:value"]["values"], ["entry:r0"])
        self.assertLess(graph.stack_transport["stats"]["iterations"], 20)

    def test_loop_backedge_clobber_prevents_first_iteration_only_proof(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "movs r3, #3", "ldr r1, [sp]",
                           "str r2, [r4]", "subs r3, #1", "bne func+0x6", "bx lr")
        self.assertNotIn("n3:value", self.aliases(graph))

    def test_balanced_push_pop_loop_tracks_sp_without_unrolling(self):
        graph = self.graph("movs r3, #3", "push {r0}", "ldr r1, [sp]", "pop {r2}",
                           "subs r3, #1", "bne func+0x2", "bx lr")
        self.assertEqual(self.aliases(graph)["n2:value"]["values"], ["entry:r0"])
        self.assertEqual(self.aliases(graph)["n3:value0"]["values"], ["entry:r0"])
        self.assertLess(graph.stack_transport["stats"]["iterations"], 20)

    def test_cyclic_reload_alias_is_explicitly_unresolved(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "ldr r1, [sp]", "str r1, [sp]",
                           "subs r2, #1", "bne func+0x4", "bx lr")
        self.assertNotIn("n2:value", self.aliases(graph))
        self.assertIn("n2:value", graph.stack_transport["unresolved_aliases"])
        self.assertTrue(any("Cyclic" in warning for warning in graph.stack_transport["warnings"]))

    def test_many_diamond_stores_widen_to_unknown_instead_of_enumerating_paths(self):
        instructions = ["sub sp, #4", "str r0, [sp]"]
        for value in range(80):
            end = (len(instructions) + 4) * 2
            instructions += ["cmp r1, #0", f"beq func+0x{end:X}", f"movs r2, #{value}", "str r2, [sp]"]
        load_index = len(instructions)
        graph = self.graph(*instructions, "ldr r3, [sp]", "bx lr")
        self.assertNotIn(f"n{load_index}:value", self.aliases(graph))
        self.assertTrue(any("bounded value" in warning for warning in graph.stack_transport["warnings"]))
        self.assertLess(graph.stack_transport["stats"]["iterations"], 5000)

    def test_join_with_different_sp_offsets_is_unknown(self):
        graph = self.graph("sub sp, #8", "str r0, [sp]", "cmp r2, #0", "beq func+0xA",
                           "add sp, #4", "ldr r3, [sp]", "bx lr")
        self.assertNotIn("n5:value", self.aliases(graph))
        self.assertIsNone(graph.stack_transport["block_sp"][-1]["entry"])

    def test_restored_known_frame_pointer_allows_new_evidence(self):
        graph = self.graph("sub sp, #8", "mov r7, sp", "mov sp, r0", "mov sp, r7", "str r1, [sp]", "ldr r2, [sp]", "bx lr")
        self.assertEqual(self.aliases(graph)["n5:value"]["values"], ["entry:r1"])

    def test_call_invalidates_even_a_known_zero_argument_signature_by_default(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "bl Callee", "ldr r1, [sp]", "bx lr", call_arities={"Callee": 0})
        self.assertNotIn("n3:value", self.aliases(graph))

    def test_private_call_transport_has_explicit_non_escape_assumptions(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "bl Callee", "ldr r1, [sp]", "bx lr",
                           call_arities={"Callee": 0}, stack_policy="abi-private")
        self.assertEqual(self.aliases(graph)["n3:value"]["values"], ["entry:r0"])
        self.assertEqual(self.aliases(graph)["n3:value"]["assumptions"], ["abi-private"])
        self.assertTrue(any("hidden" in assumption for assumption in graph.stack_transport["assumptions"]))

    def test_private_policy_cannot_assume_arity_four_has_no_stack_arguments(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "bl Callee", "ldr r1, [sp]", "bx lr",
                           call_arities={"Callee": 4}, stack_policy="abi-private")
        self.assertNotIn("n3:value", self.aliases(graph))

    def test_private_stack_address_argument_escapes(self):
        graph = self.graph("sub sp, #8", "str r1, [sp]", "mov r0, sp", "bl Callee", "ldr r2, [sp]", "bx lr",
                           call_arities={"Callee": 1}, stack_policy="abi-private")
        self.assertNotIn("n4:value", self.aliases(graph))
        self.assertTrue(any(block["escaped"] for block in graph.stack_transport["block_sp"]))

    def test_stored_then_reloaded_stack_address_is_still_an_escape(self):
        graph = self.graph("sub sp, #8", "add r0, sp, #4", "str r0, [sp]", "ldr r1, [sp]", "mov r0, r1",
                           "bl Callee", "ldr r2, [sp]", "bx lr", call_arities={"Callee": 1}, stack_policy="abi-private")
        self.assertIn("n3:value", self.aliases(graph))
        self.assertNotIn("n6:value", self.aliases(graph))

    def test_store_through_reloaded_stack_pointer_is_not_assumed_disjoint(self):
        graph = self.graph("sub sp, #12", "add r0, sp, #8", "str r0, [sp]", "ldr r1, [sp]",
                           "str r2, [sp, #8]", "str r3, [r1]", "ldr r4, [sp, #8]", "bx lr", stack_policy="abi-private")
        self.assertIn("n3:value", self.aliases(graph))
        self.assertNotIn("n6:value", self.aliases(graph))

    def test_global_stack_address_escape_survives_later_register_overwrite(self):
        graph = self.graph("sub sp, #4", "mov r0, sp", "str r0, [r1]", "movs r0, #0", "str r2, [sp]",
                           "bl Callee", "ldr r3, [sp]", "bx lr", call_arities={"Callee": 0}, stack_policy="abi-private")
        self.assertNotIn("n6:value", self.aliases(graph))

    def test_chain_of_spilled_reloads_resolves_sources_and_store_provenance(self):
        graph = self.graph("sub sp, #8", "str r0, [sp]", "ldr r1, [sp]", "str r1, [sp, #4]",
                           "ldr r2, [sp, #4]", "mov r3, r2", "bx lr")
        self.assertEqual(self.aliases(graph)["n4:value"]["values"], ["entry:r0"])
        self.assertEqual(self.aliases(graph)["n4:value"]["stores"], [2, 6])
        self.assertEqual(graph.transport_dependencies[5]["input0"], ("entry:r0",))

    def test_unknown_opcode_and_unaligned_store_invalidate(self):
        for middle in (("mystery r1",), ("mov r4, sp", "adds r4, #1", "str r1, [r4]")):
            graph = self.graph("sub sp, #8", "str r0, [sp]", *middle, "ldr r2, [sp]", "bx lr")
            self.assertEqual(self.aliases(graph), {})

    def test_deallocated_slots_do_not_resurrect_after_reallocation(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "add sp, #4", "sub sp, #4", "ldr r1, [sp]", "bx lr")
        self.assertNotIn("n4:value", self.aliases(graph))

    def test_register_overwrite_does_not_change_the_stored_value(self):
        graph = self.graph("sub sp, #4", "movs r0, #5", "str r0, [sp]", "movs r0, #7", "ldr r1, [sp]", "bx lr")
        self.assertEqual(self.aliases(graph)["n4:value"]["values"], ["n1:value"])

    def test_multiple_transfer_through_stack_alias(self):
        graph = self.graph("sub sp, #8", "mov r4, sp", "stm r4!, {r1, r0}", "subs r4, #8",
                           "ldm r4!, {r2, r3}", "bx lr")
        self.assertEqual(self.aliases(graph)["n4:value0"]["values"], ["entry:r0"])
        self.assertEqual(self.aliases(graph)["n4:value1"]["values"], ["entry:r1"])

    def test_internal_long_branch_carries_stack_evidence(self):
        graph = self.graph("sub sp, #4", "str r0, [sp]", "bl func+0xA", "str r1, [sp]", "ldr r2, [sp]", "bx lr")
        self.assertEqual(self.aliases(graph)["n4:value"]["values"], ["entry:r0"])

    def test_unknown_policy_rejected(self):
        with self.assertRaises(ValueError):
            analyze(listing("bx lr"), stack_policy="pretend-safe")


if __name__ == "__main__":
    unittest.main()
