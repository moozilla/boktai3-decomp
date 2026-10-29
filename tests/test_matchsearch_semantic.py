"""Frozen semantic call contracts, including real Thumb/Unicorn regressions.

No project ROM, ELF, or compiler build is required by these tests.
"""

from __future__ import annotations

import importlib.util
import os
from pathlib import Path
import signal
import struct
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from matchsearch.contracts import infer_call_contracts
from matchsearch.evaluator import Evaluator, _run_semantic_process, _semantic_call_arities


START = 0x08000000
CALLEE = START + 0x100
ALIAS = "declared_callee__addr"
BASELINE = "int declared_callee(int);\nint Target(int *obj) { return declared_callee((int)obj); }"
SYMBOLS = {ALIAS: (CALLEE | 1, "FUNC"), "declared_callee": (CALLEE, "FUNC"),
           "Target": (START, "FUNC")}


def thumb_call(*, unused_argument: int = 1) -> bytes:
    # push {lr}; movs r1,#unused; bl callee; pop {pc}
    delta = CALLEE - (START + 4 + 4)
    return struct.pack("<5H", 0xB500, 0x2100 | unused_argument,
                       0xF000 | ((delta >> 12) & 0x7FF),
                       0xF800 | ((delta >> 1) & 0x7FF), 0xBD00)


def fifth_argument(value: int) -> bytes:
    # Save r4/lr; reserve one outgoing word; materialize the differing value
    # in r4 (not an argument register); call; restore the stack and return.
    delta = CALLEE - (START + 10 + 4)
    return struct.pack("<9H", 0xB510, 0xB081, 0x2400 | value, 0x9400, 0x2400,
                       0xF000 | ((delta >> 12) & 0x7FF),
                       0xF800 | ((delta >> 1) & 0x7FF), 0xB001, 0xBD10)


def semantic_evaluator(source: str = BASELINE) -> tuple[Evaluator, list[str]]:
    """Only the compiler/environment boundary is fake; execution is real."""
    evaluator = Evaluator.__new__(Evaluator)
    evaluator.repo = ROOT
    evaluator.original_source = source
    evaluator.function = "Target"
    evaluator.own_names = ("Target",)
    evaluator.address = START
    evaluator.rom_symbols = dict(SYMBOLS)
    evaluator._last_preprocessed = "candidate metadata"
    evaluator.target_code = thumb_call()
    evaluator.rom = bytes(evaluator.target_code).ljust(0x1000, b"\0")
    evaluator.identity = {"physical_context": "unchanged"}
    evaluator.environment_hash = "physical-context-hash"
    preprocessed = []

    def preprocess(text):
        preprocessed.append(text)
        evaluator._last_preprocessed = text
        return text

    evaluator.preprocess = preprocess
    return evaluator, preprocessed


class SemanticContracts(unittest.TestCase):
    def test_elf_alias_and_indirect_call_name_receive_address_contract(self):
        contracts = infer_call_contracts(BASELINE, SYMBOLS)
        arities, provenance = _semantic_call_arities(contracts, {CALLEE: ALIAS})
        self.assertEqual(arities[ALIAS], 1)
        self.assertEqual(arities["via " + ALIAS], 1)
        self.assertEqual(provenance[ALIAS]["callee"], "#0x8000100")
        self.assertEqual(provenance[ALIAS]["symbols"], ["declared_callee"])

    def test_similar_alias_at_different_address_is_not_a_contract(self):
        contracts = infer_call_contracts(BASELINE, SYMBOLS)
        arities, _ = _semantic_call_arities(contracts, {CALLEE + 2: ALIAS})
        self.assertNotIn(ALIAS, arities)
        self.assertEqual(arities[f"0x{CALLEE:08X}"], 1)

    def test_unknown_and_conflicting_signatures_retain_unknown_policy(self):
        source = "int old(); int variable(int,...); int wide(long long); int a(int); int b(int,int);"
        contracts = infer_call_contracts(source, {
            "old": CALLEE, "variable": CALLEE + 2, "wide": CALLEE + 4,
            "a": CALLEE + 6, "b": CALLEE + 6})
        arities, _ = _semantic_call_arities(contracts, {
            CALLEE: "old__addr", CALLEE + 2: "variable__addr",
            CALLEE + 4: "wide__addr", CALLEE + 6: "a__addr"})
        self.assertEqual(arities, {})
        self.assertEqual(len(contracts["unknown"]), 4)

    def test_full_parameter_count_preserves_fifth_argument_observation(self):
        source = "int zero(void); int four(int,int,int,int); int five(int,int,int,int,int);"
        contracts = infer_call_contracts(source, {"zero": CALLEE, "four": CALLEE + 2, "five": CALLEE + 4})
        arities, provenance = _semantic_call_arities(contracts, {
            CALLEE: "zero", CALLEE + 2: "four", CALLEE + 4: "five"})
        self.assertEqual([arities[n] for n in ("zero", "four", "five", "via five")], [0, 4, 5, 5])
        self.assertEqual(provenance["five"]["stack_arguments"], "declared")
        self.assertEqual(provenance["four"]["stack_arguments"], "none-declared")

    def test_capped_four_without_total_count_does_not_discard_stack_arguments(self):
        arities, provenance = _semantic_call_arities({"arities": {"#0x8000100": 4}}, {CALLEE: ALIAS})
        self.assertEqual(arities[ALIAS], 5)
        self.assertEqual(provenance[ALIAS]["stack_arguments"], "unknown")

    def test_binding_is_copied_idempotent_and_cannot_change_physical_context(self):
        evaluator, _ = semantic_evaluator()
        contracts = infer_call_contracts(BASELINE, SYMBOLS)
        evaluator.bind_call_contracts(contracts)
        evaluator.bind_call_contracts(contracts)
        contracts["arities"]["#0x8000100"] = 0
        self.assertEqual(evaluator._semantic_contracts["arities"]["#0x8000100"], 1)
        with self.assertRaisesRegex(ValueError, "frozen"):
            evaluator.bind_call_contracts(contracts)
        self.assertEqual(evaluator.identity, {"physical_context": "unchanged"})
        self.assertEqual(evaluator.environment_hash, "physical-context-hash")

    def test_binding_rejects_malformed_address_or_arity_before_mutation(self):
        evaluator, _ = semantic_evaluator()
        for bad in ({"name": 1}, {"#0x8000100": True}, {"#0x8000100": 5},
                    {"#0x8000101": 1}, {"#0x100000000": 1}):
            with self.subTest(bad=bad), self.assertRaises(ValueError):
                evaluator.bind_call_contracts({"arities": bad})
        self.assertIsNone(getattr(evaluator, "_semantic_binding_hash", None))


@unittest.skipUnless(importlib.util.find_spec("unicorn"), "Unicorn is needed for executable semantic regressions")
class SemanticExecution(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        evaluator, _ = semantic_evaluator()
        result = evaluator._semantic(BASELINE, thumb_call(), 1)
        if result['verdict'] == 'unavailable':
            raise unittest.SkipTest('native backend unavailable in isolated worker: ' + result['detail'])

    def run_pair(self, before, after, prototype):
        source = prototype + '\nint Target(int *obj) { return declared_callee((int)obj); }'
        evaluator, _ = semantic_evaluator(source)
        evaluator.target_code = before
        evaluator.rom = before.ljust(0x1000, b'\0')
        return evaluator._semantic(source, after, 1)

    def check(self, evaluator, candidate_pre=BASELINE):
        return evaluator._semantic(candidate_pre, thumb_call(unused_argument=2), 1)

    def test_real_alias_reproducer_changes_false_counterexample_to_agreement(self):
        before, after = thumb_call(), thumb_call(unused_argument=2)
        unknown = self.run_pair(before, after, 'int declared_callee();')
        self.assertEqual(unknown['verdict'], 'counterexample')
        self.assertIn("r1", unknown['detail'])
        known = self.run_pair(before, after, 'int declared_callee(int);')
        self.assertEqual(known['verdict'], 'sampled_agree')
        self.assertEqual(known['isolation']['mode'], 'subprocess')

    def test_real_fifth_argument_difference_survives_capped_register_contract(self):
        result = self.run_pair(fifth_argument(1), fifth_argument(2), 'int declared_callee(int,int,int,int,int);')
        self.assertEqual(result['verdict'], 'counterexample')
        self.assertIn("stack argument sp+0 differs", result['detail'])
        # A naive direct copy of the capped graph arity loses that observation.
        hidden = self.run_pair(fifth_argument(1), fifth_argument(2), 'int declared_callee(int,int,int,int);')
        self.assertEqual(hidden['verdict'], 'sampled_agree')

    def test_candidate_prototypes_cannot_change_alias_contract_or_input_sampling(self):
        evaluator, preprocessed = semantic_evaluator()
        candidate = "int declared_callee(int,int,int,int); void Target(int obj) { declared_callee(obj,0,0,0); }"
        result = self.check(evaluator, candidate)
        self.assertEqual(result["verdict"], "sampled_agree")
        self.assertEqual(result["input_contract"]["parameter_kinds"], ["p"])
        self.assertTrue(result["input_contract"]["check_return_register"])
        called = {c["name"]: c for c in result["call_contracts"]["observed_target_callees"]}
        self.assertEqual(called[ALIAS]["register_arguments"], 1)
        self.assertEqual(called[ALIAS]["symbols"], ["declared_callee"])
        self.assertEqual(result["model_version"], 3)
        self.assertFalse(result["proof"])
        self.assertEqual(evaluator._last_preprocessed, "candidate metadata")
        self.check(evaluator, "void declared_callee(void); void Target(void) {}")
        self.assertEqual(preprocessed, [BASELINE])
        self.assertEqual(result['isolation']['returncode'], 0)

    def test_candidate_added_prototype_cannot_narrow_an_unknown_baseline(self):
        baseline = BASELINE.replace("declared_callee(int);", "declared_callee();")
        evaluator, _ = semantic_evaluator(baseline)
        result = self.check(evaluator, BASELINE)
        self.assertEqual(result["verdict"], "counterexample")
        self.assertIn("r1", result["detail"])
        called = {c["name"]: c for c in result["call_contracts"]["observed_target_callees"]}
        self.assertEqual(called[ALIAS]["basis"], "no frozen address contract")
        self.assertGreater(result["call_contracts"]["unknown_declarations"], 0)

    def test_session_can_bind_its_stored_baseline_instead_of_current_source_file(self):
        evaluator, preprocessed = semantic_evaluator("void Target(void) {}")
        evaluator.bind_call_contracts(infer_call_contracts(BASELINE, SYMBOLS), baseline_source=BASELINE)
        result = self.check(evaluator)
        self.assertEqual(result["verdict"], "sampled_agree")
        self.assertEqual(result["input_contract"]["parameter_kinds"], ["p"])
        self.assertEqual(preprocessed, [BASELINE])

    def test_mismatched_baseline_provenance_is_inconclusive(self):
        evaluator, _ = semantic_evaluator()
        evaluator.bind_call_contracts(infer_call_contracts(BASELINE, SYMBOLS), baseline_source="void Target(void) {}")
        result = self.check(evaluator)
        self.assertEqual(result["verdict"], "inconclusive")
        self.assertIn("do not match", result["detail"])
        self.assertFalse(result["proof"])
        self.assertIsNone(evaluator._semantic_input)


class SemanticIsolation(unittest.TestCase):
    @unittest.skipUnless(os.name == 'posix', 'POSIX signal regression')
    def test_actual_native_signal_preserves_parent_and_frozen_contract_metadata(self):
        command = [sys.executable, '-c',
                   'import os,signal,resource;resource.setrlimit(resource.RLIMIT_CORE,(0,0));'
                   'os.kill(os.getpid(),signal.SIGILL)']
        evaluator, _ = semantic_evaluator()
        run = _run_semantic_process

        def crash(request, **kwargs):
            # Fault the actual subprocess boundary, not a mocked return code.
            return run(request, command=command, **kwargs)

        with patch('matchsearch.evaluator._run_semantic_process', side_effect=crash):
            result = evaluator._semantic(BASELINE, thumb_call(), 1)
        self.assertEqual(result['verdict'], 'unavailable')
        self.assertEqual(result['isolation']['signal'], signal.SIGILL)
        self.assertEqual(result['trials'], 0)
        self.assertFalse(result['proof'])
        self.assertEqual(result['call_contracts']['known_addresses'], 2)
        self.assertEqual(result['call_contracts']['binding_sha256'], evaluator._semantic_binding_hash)

    def test_actual_hung_child_is_bounded(self):
        command = [sys.executable, '-c', 'import time; time.sleep(10)']
        result = _run_semantic_process({}, cwd=ROOT, timeout=0.05, command=command)
        self.assertEqual(result['verdict'], 'unavailable')
        self.assertEqual(result['isolation']['termination'], 'timeout')
        self.assertFalse(result['proof'])

    def test_malformed_response_is_not_a_semantic_verdict(self):
        command = [sys.executable, '-c', 'print("not JSON")']
        result = _run_semantic_process({}, cwd=ROOT, command=command)
        self.assertEqual(result['verdict'], 'unavailable')
        self.assertIn('Invalid semantic worker response', result['detail'])


if __name__ == "__main__":
    unittest.main()
