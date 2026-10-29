"""Boundary evidence must not hide changed producers outside a local region."""
from __future__ import annotations

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.matchsearch.boundaries import align_regions, compare_boundaries
from tools.matchsearch.constraints import analyze


def graph(lines, *, stack_policy="strict"):
    return analyze("\n".join(f"{offset:04x} {text}" for offset, text in lines),
                   call_arities={"#0x9000": 0, "#0x9004": 1}, stack_policy=stack_policy)


def test_external_calls_anchor_repeated_regions_without_crossing():
    a = graph([(0, "movs r4, #1"), (2, "bl #0x9000"), (6, "movs r4, #2"),
               (8, "bl #0x9000"), (12, "mov r0, r4"), (14, "bx lr")])
    b = graph([(0, "movs r5, #1"), (2, "movs r6, #8"), (4, "bl #0x9000"),
               (8, "movs r5, #2"), (10, "bl #0x9000"), (14, "mov r0, r5"), (16, "bx lr")])
    pairs, regions, evidence = align_regions(a, b)
    assert evidence["matched_calls"] == 2
    assert evidence["call_sequence_equal"]
    assert evidence["repeated_callees"] == 1
    assert (1, 2) in pairs and (3, 4) in pairs
    assert regions[0]["target_indices"] == (0, 2)
    assert not evidence["proof"]


def test_identical_local_instructions_do_not_hide_different_external_value():
    a = graph([(0, "movs r4, #1"), (2, "bl #0x9000"), (6, "mov r0, r4"),
               (8, "bl #0x9004"), (12, "bx lr")])
    b = graph([(0, "movs r4, #2"), (2, "bl #0x9000"), (6, "mov r0, r4"),
               (8, "bl #0x9004"), (12, "bx lr")])
    region = compare_boundaries(a, b, region="call:1", detail_limit=30)["regions"][0]
    assert region["changed_operands"] == 0
    assert region["unmatched_instructions"] == 0
    assert region["transported_inputs"]["different"]
    assert any(r["status"] == "different" and "entry" not in str(r["target_values"])
               for r in region["input_obligations"])
    assert region["instruction_correspondence"].get("different")
    assert region["status"] == "open"


def test_identity_has_corresponding_boundary_obligations():
    a = graph([(0, "movs r4, #1"), (2, "bl #0x9000"), (6, "mov r0, r4"),
               (8, "bl #0x9004"), (12, "bx lr")])
    report = compare_boundaries(a, a, include_equal=True)
    assert report["structural_changes"] == {"unmatched_instructions": 0, "changed_operands": 0}
    assert all(row["status"] == "corresponding-under-alignment" for row in report["regions"])
    assert not report["proof"]


def test_internal_operand_swap_stays_open_even_without_crossing_value_changes():
    # These return -1 and +1. All operation shapes and region inputs agree;
    # the changed subtraction dependencies and return are internal to the tail.
    a = graph([(0, "movs r0, #1"), (2, "movs r1, #2"),
               (4, "subs r0, r0, r1"), (6, "bx lr")])
    b = graph([(0, "movs r0, #1"), (2, "movs r1, #2"),
               (4, "subs r0, r1, r0"), (6, "bx lr")])
    report = compare_boundaries(a, b)
    assert report["structural_changes"] == {"unmatched_instructions": 0, "changed_operands": 0}
    region = report["regions"][0]  # Default visibility must retain the conflict.
    assert region["transported_inputs"] == {"corresponding": 5}
    assert region["transported_outputs"] == {}
    assert region["instruction_correspondence"]["different"]
    assert region["status"] == "open"


def test_identical_unsupported_instruction_is_unresolved_and_remains_visible():
    a = graph([(0, "mystery r0, r1"), (2, "bx lr")])
    region = compare_boundaries(a, a)["regions"][0]
    assert region["unmatched_instructions"] == region["changed_operands"] == 0
    assert region["instruction_correspondence"]["unresolved"]
    assert region["status"] == "open"


def test_internal_branch_destination_change_stays_open():
    a = graph([(0, "movs r0, #0"), (2, "cmp r0, #0"), (4, "beq func+0xA"),
               (6, "movs r0, #1"), (8, "b func+0xC"), (10, "movs r0, #2"), (12, "bx lr")])
    b = graph([(0, "movs r0, #0"), (2, "cmp r0, #0"), (4, "beq func+0x6"),
               (6, "movs r0, #1"), (8, "b func+0xC"), (10, "movs r0, #2"), (12, "bx lr")])
    region = compare_boundaries(a, b)["regions"][0]
    assert region["unmatched_instructions"] == region["changed_operands"] == 0
    assert region["instruction_correspondence"].get("different")
    assert region["status"] == "open"


def test_internal_correspondence_uses_stack_values_without_hiding_physical_difference():
    # Both loads read the same saved value; their destination order changes
    # which physical load supplies arg0 without changing its transported value.
    common = [(0, "sub sp, #4"), (2, "movs r4, #7"), (4, "str r4, [sp]")]
    tail = [(10, "bl #0x9004"), (14, "add sp, #4"), (16, "bx lr")]
    a = graph(common + [(6, "ldr r0, [sp]"), (8, "ldr r5, [sp]")] + tail)
    b = graph(common + [(6, "ldr r5, [sp]"), (8, "ldr r0, [sp]")] + tail)
    region = compare_boundaries(a, b, region="call:0")["regions"][0]
    assert region["physical_constraints"].get("different")
    assert not region["instruction_correspondence"].get("different")
    assert not region["instruction_correspondence"].get("unresolved")
    assert region["status"] == "corresponding-under-alignment"


def test_local_data_pointers_have_unresolved_destinations_and_remain_visible():
    data = [(4, ".word 0x11111111"), (8, ".word 0x22222222"), (12, ".word 0x33333333")]
    a = graph([(0, "ldr r0, =func+0x8"), (2, "bx lr")] + data)
    b = graph([(0, "ldr r0, =func+0xC"), (2, "bx lr")] + data)
    region = compare_boundaries(a, b)["regions"][0]
    assert region["unmatched_instructions"] == region["changed_operands"] == 0
    assert region["instruction_correspondence"].get("unresolved")
    assert region["status"] == "open"


def test_local_code_pointers_to_different_mapped_instructions_do_not_correspond():
    tail = [(2, "bx lr"), (4, "nop"), (6, "nop")]
    a = graph([(0, "ldr r0, =func+0x4")] + tail)
    b = graph([(0, "ldr r0, =func+0x6")] + tail)
    region = compare_boundaries(a, b)["regions"][0]
    assert region["instruction_correspondence"].get("different")
    assert region["status"] == "open"


def test_relocated_local_code_pointers_can_correspond_under_instruction_mapping():
    a = graph([(0, "ldr r0, =func+0x4"), (2, "b func+0x4"),
               (4, "movs r0, #1"), (6, "bx lr")])
    b = graph([(0, "ldr r0, =func+0x8"), (2, "b func+0x8"), (4, ".word 0x12345678"),
               (8, "movs r0, #1"), (10, "bx lr")])
    region = compare_boundaries(a, b, include_equal=True)["regions"][0]
    assert region["instruction_correspondence"] == {"corresponding": 4}
    assert region["status"] == "corresponding-under-alignment"
    assert not region["proof"]


def test_local_bl_is_not_a_call_anchor():
    a = graph([(0, "movs r4, #1"), (2, "bl func+0x8"), (6, "movs r4, #2"),
               (8, "mov r0, r4"), (10, "bx lr")])
    _, _, evidence = align_regions(a, a)
    assert evidence["target_calls"] == 0


def test_inserted_unpaired_value_at_boundary_remains_unresolved():
    a = graph([(0, "movs r4, #1"), (2, "bl #0x9000"), (6, "mov r0, r4"),
               (8, "bl #0x9004"), (12, "bx lr")])
    b = graph([(0, "movs r4, #1"), (2, "bl #0x9000"), (6, "muls r4, r5"),
               (8, "mov r0, r4"), (10, "bl #0x9004"), (14, "bx lr")])
    report = compare_boundaries(a, b, include_equal=True, detail_limit=40)
    assert any(row["transported_inputs"].get("unresolved") for row in report["regions"])


SPILLED = [(0, "sub sp, #4"), (2, "movs r4, #7"), (4, "str r4, [sp]"),
           (6, "bl #0x9000"), (10, "ldr r0, [sp]"), (12, "bl #0x9004"),
           (16, "add sp, #4"), (18, "bx lr")]
HELD = [(0, "sub sp, #4"), (2, "movs r4, #7"), (4, "bl #0x9000"),
        (8, "mov r0, r4"), (10, "bl #0x9004"), (14, "add sp, #4"), (16, "bx lr")]


def test_reload_and_held_register_correspond_only_with_stated_stack_contract():
    strict = compare_boundaries(graph(SPILLED), graph(HELD), region="call:1", detail_limit=100)
    strict_arg = next(row for row in strict["regions"][0]["input_obligations"] if row["role"] == "arg0")
    assert strict_arg["status"] != "corresponding"
    conditional = compare_boundaries(graph(SPILLED, stack_policy="abi-private"),
                                     graph(HELD, stack_policy="abi-private"), region="call:1", detail_limit=100)
    region = conditional["regions"][0]
    arg = next(row for row in region["input_obligations"] if row["role"] == "arg0")
    assert arg["status"] == "corresponding"
    assert arg["target_transport"][0]["stores"] == [4]
    assert arg["target_transport"][0]["assumptions"] == ["abi-private"]
    assert arg["candidate_transport"] == []
    assert any("hidden pointer" in assumption for assumption in conditional["assumptions"])
    assert region["physical_constraints"].get("different", 0) + region["physical_constraints"].get("unresolved", 0) > 0
    assert not conditional["proof"]


def test_transport_does_not_hide_a_changed_stored_value_producer():
    changed = [(offset, "movs r4, #8" if offset == 2 else text) for offset, text in HELD]
    report = compare_boundaries(graph(SPILLED, stack_policy="abi-private"),
                                graph(changed, stack_policy="abi-private"), region="call:1", detail_limit=100)
    arg = next(row for row in report["regions"][0]["input_obligations"] if row["role"] == "arg0")
    assert arg["status"] == "different"
    assert arg["target_transport"]


def test_differing_stack_policies_are_visible_and_assumptions_are_merged():
    report = compare_boundaries(graph(SPILLED), graph(SPILLED, stack_policy="abi-private"), include_equal=True)
    assert report["stack_policy"] == "mixed"
    assert report["stack_policies"] == {"target": "strict", "candidate": "abi-private"}
    assert any(assumption.startswith("abi-private:") for assumption in report["assumptions"])



def load_tests(loader, tests, pattern):
    """Discover these fixture-free regressions with the standard library too."""
    import unittest
    for name, function in sorted(globals().items()):
        if name.startswith('test_') and callable(function):
            tests.addTest(unittest.FunctionTestCase(function))
    return tests


if __name__ == '__main__':
    import unittest
    unittest.main()
