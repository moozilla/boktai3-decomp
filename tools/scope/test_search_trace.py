"""Compiler snapshot and cross-node correspondence guarantees."""
from pathlib import Path
import re
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
import search_trace as trace


class FakeScope:
    @staticmethod
    def one_line(rtl):
        return re.sub(r"^\((?:insn|jump_insn|call_insn) \d+ \d+ \d+ ", "", rtl).strip()


class TraceTests(unittest.TestCase):
    def normalize(self, text, identities=None):
        with tempfile.TemporaryDirectory() as d:
            Path(d, "u.i.rtl").write_text(text)
            return trace._normalize_pass(FakeScope, d, "rtl", identities or {})

    def test_pseudo_and_uid_renumbering_does_not_change_pass_hash(self):
        a = self.normalize('(insn 4 3 5 (set (reg:SI 22) (reg:SI 0 r0)))\n\n(insn 5 4 6 (set (reg:SI 23) (plus:SI (reg:SI 22) (const_int 1))))\n')
        b = self.normalize('(insn 40 30 50 (set (reg:SI 122) (reg:SI 0 r0)))\n\n(insn 50 40 60 (set (reg:SI 123) (plus:SI (reg:SI 122) (const_int 1))))\n')
        self.assertEqual(a['hash'], b['hash'])

    def test_register_dependency_change_is_retained(self):
        prefix = '(insn 4 3 5 (set (reg:SI 22) (reg:SI 0 r0)))\n\n'
        a = self.normalize(prefix + '(insn 5 4 6 (set (reg:SI 23) (plus:SI (reg:SI 22) (const_int 1))))\n')
        b = self.normalize(prefix + '(insn 5 4 6 (set (reg:SI 23) (plus:SI (reg:SI 24) (const_int 1))))\n')
        self.assertNotEqual(a['hash'], b['hash'])

    def test_hard_register_binding_is_retained(self):
        a = self.normalize('(insn 1 0 2 (set (reg:SI 0 r0) (const_int 1)))\n')
        b = self.normalize('(insn 1 0 2 (set (reg:SI 1 r1) (const_int 1)))\n')
        self.assertNotEqual(a['hash'], b['hash'])

    def test_branch_target_position_is_retained(self):
        source = '(jump_insn 1 0 2 (set (pc) (label_ref 10)))\n\n(code_label 10 1 3 1 "")\n\n(insn 3 10 20 (set (reg:SI 0 r0) (const_int 1)))\n\n(code_label 20 3 0 2 "")\n'
        a = self.normalize(source)
        b = self.normalize(source.replace('(label_ref 10)', '(label_ref 20)'))
        self.assertNotEqual(a['hash'], b['hash'])
        self.assertEqual(a['label_count'], 2)

    def test_source_names_survive_pseudo_renumbering(self):
        a = self.normalize('(insn 1 0 2 (set (reg/v:SI 22) (const_int 1)))\n', {22: {'name': 'counter', 'key': 'counter#1', 'ambiguous': False}})
        b = self.normalize('(insn 1 0 2 (set (reg/v:SI 88) (const_int 1)))\n', {88: {'name': 'counter', 'key': 'counter#1', 'ambiguous': False}})
        self.assertEqual(a['hash'], b['hash'])

    def test_delta_matches_name_not_pseudo(self):
        a = {'allocations': [{'key': 'p#1', 'pseudo': 22, 'hard_register': 4, 'refs': 8, 'priority': 40}], 'reloads': [], 'trace_matches_build': True}
        b = {'allocations': [{'key': 'p#1', 'pseudo': 44, 'hard_register': 5, 'refs': 10, 'priority': 50}], 'reloads': [], 'trace_matches_build': True}
        delta = trace.delta(a, b)
        self.assertEqual(len(delta['allocations']), 1)
        change = delta['allocations'][0]
        self.assertEqual(change['key'], 'p#1')
        self.assertEqual(change['changes']['hard_register'], {'parent': 4, 'child': 5})
        self.assertNotIn('pseudo', change['changes'])

    def test_ambiguous_correspondence_stays_explicit(self):
        a = {'allocations': [{'key': 'v#2', 'hard_register': 4, 'ambiguous': True}]}
        b = {'allocations': [{'key': 'v#2', 'hard_register': 5, 'ambiguous': True}]}
        self.assertTrue(trace.delta(a, b)['allocations'][0]['ambiguous'])

    def test_earliest_changed_pass_is_ordered_and_source_linked(self):
        a = {'pass_order': ['rtl', 'cse', 'greg'], 'passes': {'rtl': {'hash': 'a'}, 'cse': {'hash': 'b'}, 'greg': {'hash': 'c'}}}
        b = {'pass_order': ['rtl', 'cse', 'greg'], 'passes': {'rtl': {'hash': 'a'}, 'cse': {'hash': 'x', 'compiler': {'reference': 'cse.c:8612'}}, 'greg': {'hash': 'z'}}}
        result = trace.delta(a, b)
        self.assertEqual(result['earliest_changed_pass']['pass'], 'cse')
        self.assertEqual(result['earliest_changed_pass']['compiler']['reference'], 'cse.c:8612')
        self.assertEqual([x['pass'] for x in result['changed_passes']], ['cse', 'greg'])

    def test_diagnostic_build_mismatch_cannot_be_hidden(self):
        self.assertFalse(trace.delta({'trace_matches_build': True}, {'trace_matches_build': False})['trace_matches_build'])


if __name__ == '__main__':
    unittest.main()
