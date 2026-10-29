"""Large repeated store motifs need bounded, non-greedy correspondence."""
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.matchsearch.boundaries import _align_region
from tools.matchsearch.constraints import analyze


def instructions(lines):
    return analyze("\n".join(f"{2*i:04x} {line}" for i, line in enumerate(lines))).instructions


def test_changed_constants_do_not_shift_an_entire_repeated_store_motif():
    target = instructions(["movs r4, #1", "str r4, [r0]", "movs r4, #2", "str r4, [r0]"])
    candidate = instructions(["movs r5, #3", "str r5, [r0]", "movs r5, #1", "str r5, [r0]"])
    pairs, method = _align_region(target, candidate)
    assert method == "bounded-minimum-edit"
    assert pairs == [(0, 0), (1, 1), (2, 2), (3, 3)]
    assert sum(target[x].shape != candidate[y].shape for x, y in pairs) == 2


def test_alignment_budget_falls_back_explicitly_and_covers_every_instruction():
    target = instructions(["movs r4, #1", "str r4, [r0]"])
    candidate = instructions(["movs r5, #2", "str r5, [r0]", "adds r5, #1"])
    pairs, method = _align_region(target, candidate, cell_limit=1)
    assert method == "greedy-fallback"
    assert [x for x, _ in pairs if x is not None] == list(range(len(target)))
    assert [y for _, y in pairs if y is not None] == list(range(len(candidate)))


def test_changed_opcode_is_not_presented_as_only_a_changed_scalar_operand():
    target = instructions(["lsls r4, r5, #1", "str r4, [r0]"])
    candidate = instructions(["lsrs r4, r5, #1", "str r4, [r0]"])
    pairs, _ = _align_region(target, candidate)
    assert (0, 0) not in pairs
    assert (1, 1) in pairs
    assert len([pair for pair in pairs if None in pair]) == 2



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
