"""Evidence boundaries for reload demand reconstruction, without solved functions."""
from pathlib import Path
import tempfile
import unittest

from search_reload import analyze, _liveness, _reservations


LREG = ''';; Start of basic block 0, registers live: 22
(insn 10 0 20 (set (reg:SI 30)
        (plus:SI (reg/v:SI 22) (const_int 1))) 0 (nil)
    (expr_list:REG_UNUSED (reg:SI 30) (nil)))

(insn 20 10 0 (set (reg:SI 31)
        (plus:SI (reg/v:SI 22) (const_int 4))) 0 (nil)
    (expr_list:REG_DEAD (reg/v:SI 22) (nil)))

;; End of basic block 0
'''
GREG = '''Spilling for insn 10.
Spilling reg 0.
Spilling reg 5.
Spilling for insn 20.
Spilling reg 0.
Spilling reg 5.
Spilling reg 6.
Spilling for insn 10.
Spilling reg 0.
Spilling reg 1.
Spilling for insn 21.
'''


class FakeScope:
    def __init__(self, root):
        self.INSTR = str(root / 'gcc' / 'agbcc')
        self.choices = [(0, 10, 0, 4, 'reg22'), (1, 10, 5, 4, '=1023'),
                        (2, 20, 6, 8, 'reg9')]
        self.states = {
            0: {'uid': 10, 'start': -1, 'pass': 1, 'regs': [0, 1, 5, 6],
                'st': {0: 'A', 1: 'L40', 5: 'A', 6: 'A'}},
            1: {'uid': 10, 'start': 0, 'pass': 1, 'regs': [0, 1, 5, 6],
                'st': {0: 'T', 1: 'L40', 5: 'A', 6: 'A'}},
            2: {'uid': 20, 'start': 2, 'pass': 1, 'regs': [0, 1, 5, 6],
                'st': {0: 'T', 1: 'H', 5: 'T', 6: 'A'}},
        }

    def dump_func(self, wd, function):
        return function

    def pseudo_names(self, wd, function):
        return {22: 'position', 23: 'center', 40: 'index'}

    def uid_lines(self, wd):
        return {10: 4, 20: 8}

    def alloc_records(self, wd, function):
        return {22: ('G', 1, 100, 8, 100, 2, 5),
                23: ('G', 2, 50, 4, 100, 2, -1)}

    def insn_list(self, wd, pass_name):
        if pass_name != 'lreg':
            return []
        return [(10, 'insn', '(set (reg:SI 30) (plus:SI (reg:SI 22) (const_int 1)))'),
                (20, 'insn', '(set (reg:SI 31) (mem:SI (reg:SI 22)))')]

    def source_line_map(self, text):
        return {4: 'position->x += 1;', 8: 'Read(position);'}

    def spill_states(self, wd, function):
        return self.states

    def reload_records(self, wd, function):
        return self.choices

    def inherit_records(self, wd, function):
        return [(21, 22, 5, 10, 'inherit')]

    def frame_slots(self, wd, function):
        return {16: (22, 4, 1, 5), 20: (23, 8, 2, -1)}

    def inventory(self):
        return {}, {'Example': 0}, {'Example': 8}

    def rom_bytes(self):
        return b'12345678'

    def off(self, address):
        return address

    def rom_data(self, function, address, target):
        return []

    def decode(self, target, address, data):
        return [(0, 2, 'push {r4, r5, r6, r7, lr}', '', None),
                (2, 2, 'mov r7, sl', '', None),
                (4, 2, 'adds r0, r1, #1', '', None),
                (6, 2, 'pop {r4, r5, r6, r7}', '', None)]

    def reg_nums(self, text):
        import re
        return [int(n) for n in re.findall(r'\br(\d+)\b', text)]


class ReloadAnalysisTests(unittest.TestCase):
    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory(prefix='reload-query-test-')
        self.addCleanup(self.scratch.cleanup)
        self.wd = Path(self.scratch.name)
        (self.wd / 'u.i.lreg').write_text(LREG)
        (self.wd / 'u.i.greg').write_text(GREG)
        (self.wd / 'u.i').write_text('')
        (self.wd / 'gcc').mkdir()
        (self.wd / 'gcc' / 'reload1.c').write_text('''find_reload_regs (chain)
short *group_needs = chain->need.groups;
maybe_mark_pseudo_spilled (reg)
SET_REGNO_REG_SET (spilled_pseudos, reg);
''')
        self.scope = FakeScope(self.wd)

    def test_native_reservations_preserve_repeated_scans_and_zero_needs(self):
        scans = _reservations(GREG)
        self.assertEqual([s['uid'] for s in scans], [10, 20, 10, 21])
        self.assertEqual(scans[0]['registers'], [0, 5])
        self.assertEqual(scans[-1]['registers'], [])
        result = analyze(self.scope, self.wd, 'Example')
        self.assertEqual(result['summary']['max_per_insn_reserved_registers'], 3)
        self.assertEqual(result['observed_selection_union_growth'][1]['first_observed_registers'], [6])
        self.assertEqual(result['observed_selection_union_growth'][2]['first_observed_registers'], [1])
        self.assertFalse(result['missing_instrumentation']['exact_need_vectors']['available'])

    def test_overlap_bound_uses_taken_periods_not_successful_choice_count(self):
        result = analyze(self.scope, self.wd, 'Example')
        at = {item['uid']: item for item in result['instructions']}
        self.assertEqual(at[10]['successful_choices'], 2)
        self.assertEqual(at[10]['overlap_lower_bound'], 2)
        self.assertEqual(at[20]['successful_choices'], 1)
        self.assertEqual(at[20]['overlap_lower_bound'], 3)
        self.assertEqual(result['pressure_points'][0]['uid'], 20)
        blocker = at[10]['choices'][0]['live_pseudo_blockers'][0]
        self.assertEqual(blocker, {'register': 1, 'pseudo': 40, 'name': 'index'})
        self.assertEqual(at[21]['successful_choices'], 0)
        self.assertEqual(at[21]['inheritance'][0]['from_uid'], 10)

    def test_final_eviction_is_actual_but_localizing_its_cause_is_inferred(self):
        result = analyze(self.scope, self.wd, 'Example')
        eviction = result['evictions'][0]
        self.assertEqual(eviction['spilled_from_register'], 5)
        self.assertEqual(eviction['name'], 'position')
        self.assertEqual(eviction['earliest_candidate_selection']['uid'], 10)
        self.assertEqual(eviction['earliest_candidate_selection']['line'], 4)
        self.assertFalse(eviction['cause_proved'])
        self.assertEqual(result['unallocated_stack_slots'][0]['name'], 'center')
        self.assertFalse(result['missing_instrumentation']['exact_victim_event']['available'])
        self.assertEqual(result['missing_instrumentation']['exact_victim_event']['record_at']['line'], 4)

    def test_lifetime_recovers_block_live_ins_and_death_notes(self):
        live = _liveness(LREG)
        self.assertIn(22, live[10]['before'])
        self.assertIn(22, live[20]['before'])
        self.assertNotIn(22, live[20]['after'])
        self.assertNotIn(30, live[10]['after'])
        self.assertEqual(_liveness('(insn 10 0 0 (set (reg:SI 22) (const_int 1)))'), {})

    def test_missing_records_do_not_become_false_zero_demand_proof(self):
        (self.wd / 'u.i.greg').write_text('')
        (self.wd / 'u.i.lreg').write_text('')
        self.scope.choices = []
        self.scope.states = {}
        result = analyze(self.scope, self.wd, 'Example')
        self.assertEqual(len(result['evidence_missing']), 3)
        self.assertFalse(result['missing_instrumentation']['exact_need_vectors']['available'])
        self.assertIsNone(result['evictions'][0]['earliest_candidate_selection'])

    def test_target_nonuse_is_labeled_observation_not_original_spill_evidence(self):
        result = analyze(self.scope, self.wd, 'Example')
        observed = result['target_register_observation']
        self.assertEqual(observed['body_low_registers_used'], [0, 1])
        self.assertIn(7, observed['body_low_registers_not_seen'])
        self.assertIn('does not prove', observed['interpretation'])
        self.assertEqual(result['summary']['final_spill_registers'], [0, 1, 5, 6])

    def test_machine_constraints_preserve_earlyclobber_without_guessing_alternative(self):
        (self.wd / 'u.i.lreg').write_text(LREG.replace('0 (nil)', '31 {multiply} (nil)', 1))
        (self.wd / 'gcc' / 'thumb.md').write_text('''(define_insn "multiply"
  [(set (match_operand:SI 0 "s_register_operand" "=&l,&l")
        (mult:SI (match_operand:SI 1 "s_register_operand" "%l,0")
                 (match_operand:SI 2 "s_register_operand" "l,l")))
   (clobber (match_scratch:SI 3 "=&l"))]
  "" "mul")
''')
        result = analyze(self.scope, self.wd, 'Example')
        pattern = next(i for i in result['instructions'] if i['uid'] == 10)['pre_reload']['machine_pattern']
        self.assertEqual(pattern['name'], 'multiply')
        self.assertEqual(pattern['constraints'][0]['constraint'], '=&l,&l')
        self.assertEqual(pattern['constraints'][-1]['kind'], 'scratch')
        self.assertIn('chosen alternative is not recorded', pattern['evidence'])

    def test_multiword_eviction_and_reused_stack_slot_are_not_lost(self):
        (self.wd / 'u.i.greg').write_text('Spilling for insn 10.\nSpilling reg 7.\n')
        (self.wd / 'names.txt').write_text('S Example 22 16 8 1 6\nS Example 23 16 8 2 -1\n')
        result = analyze(self.scope, self.wd, 'Example')
        self.assertEqual(len(result['evictions']), 1)
        self.assertEqual(len(result['unallocated_stack_slots']), 1)
        eviction = result['evictions'][0]
        self.assertEqual(eviction['spilled_from_registers'], [6, 7])
        self.assertEqual(eviction['earliest_candidate_selection']['selected_registers'], [7])

    def test_preexisting_stack_argument_is_not_a_hard_register_eviction(self):
        (self.wd / 'names.txt').write_text('S Example 22 32 4 -1 0\n')
        result = analyze(self.scope, self.wd, 'Example')
        self.assertEqual(result['evictions'], [])
        argument = result['unallocated_stack_slots'][0]
        self.assertEqual(argument['slot_kind'], 'incoming_or_preexisting_stack')
        self.assertFalse(argument['from_register_valid'])


if __name__ == '__main__':
    unittest.main()
