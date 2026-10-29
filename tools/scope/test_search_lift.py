"""Address lifting safety: observable identities, joins, loops, calls, and spills."""
import json
import unittest

from search_lift import lift, compare


def rows(*instructions):
    return [(2 * n, 2, text, text, None) for n, text in enumerate(instructions)]


class SearchLiftTests(unittest.TestCase):
    def test_resolved_offset_and_register_independent_identity(self):
        target = lift(rows('mov r4,r0', 'movs r1,#1', 'lsls r1,r1,#10', 'adds r4,r4,r1', 'ldrh r2,[r4,#16]'))
        current = lift(rows('mov r6,r0', 'ldr r3,=0x410', 'adds r6,r6,r3', 'ldrh r7,[r6]'))
        self.assertEqual(target['memory'][0]['address']['text'], 'arg0 + 0x410')
        self.assertEqual(len(compare(target, current)['paired']), 1)
        self.assertEqual(target['memory'][0]['width'], 2)
        self.assertFalse(target['memory'][0]['signed'])
        json.dumps(target)

    def test_global_index_and_store_source_value(self):
        report = lift(rows('ldr r4,=0x02030000', 'lsls r1,r1,#2', 'adds r4,r4,r1', 'adds r4,#16', 'str r2,[r4,#8]'))
        access, = report['memory']
        self.assertEqual(access['address']['displacement'], 0x02030018)
        self.assertEqual(access['address']['terms'][0]['scale'], 4)
        self.assertEqual(access['value']['text'], 'arg2')

    def test_join_preserves_ambiguity(self):
        report = lift(rows('cmp r2,#0', 'beq func+0x8', 'mov r4,r0', 'b func+0xa', 'mov r4,r1', 'ldr r3,[r4]'))
        access, = report['memory']
        self.assertIn('arg0', access['address']['text'])
        self.assertIn('arg1', access['address']['text'])
        self.assertEqual(access['address']['certainty'], 'ambiguous')
        self.assertIsNone(access['identity_key'])

    def test_affine_loop_stride_is_derived_from_backedge_equation(self):
        report = lift(rows('mov r4,r0', 'movs r1,#0', 'ldrh r2,[r4,#16]', 'adds r4,#8',
                           'adds r1,#1', 'cmp r1,#3', 'blt func+0x4', 'bx lr'))
        access, = report['memory']
        self.assertIn('iteration[loop0]', access['address']['text'])
        self.assertIn('8*iteration', access['address']['text'])
        self.assertEqual(access['address']['displacement'], 16)
        induction = [e for e in report['loops'][0]['equations'] if e['register'] == 'r4']
        self.assertEqual(induction[0]['stride'], 8)
        self.assertTrue(report['summary']['fixed_point_complete'])

    def test_loop_stride_can_be_held_in_a_higher_numbered_invariant_register(self):
        report = lift(rows('mov r4,r0', 'movs r7,#8', 'ldrh r2,[r4,#16]', 'adds r4,r4,r7',
                           'cmp r4,r1', 'blo func+0x4', 'bx lr'))
        access, = report['memory']
        self.assertIn('8*iteration[loop0]', access['address']['text'])
        self.assertEqual(access['address']['certainty'], 'symbolic')

    def test_nested_loop_invariant_projects_into_outer_induction(self):
        report = lift(rows('mov r4,r0', 'movs r1,#0', 'movs r2,#0', 'ldrh r3,[r4,#16]',
                           'adds r2,#1', 'cmp r2,#2', 'blt func+0x6', 'adds r4,#8',
                           'adds r1,#1', 'cmp r1,#3', 'blt func+0x4', 'bx lr'))
        access, = report['memory']
        self.assertIn('8*iteration[loop0]', access['address']['text'])
        self.assertNotIn('iteration[loop1]', access['address']['text'])
        self.assertEqual(access['address']['certainty'], 'symbolic')
        self.assertTrue(report['summary']['fixed_point_complete'])

    def test_loop_reset_is_not_mistaken_for_induction(self):
        report = lift(rows('mov r4,r0', 'movs r1,#0', 'mov r4,r0', 'adds r4,#8',
                           'ldrh r2,[r4,#16]', 'adds r1,#1', 'cmp r1,#3', 'blt func+0x4', 'bx lr'))
        access, = report['memory']
        self.assertEqual(access['address']['text'], 'arg0 + 0x18')
        self.assertFalse(any(e['register'] == 'r4' and e['status'] == 'affine-induction' for e in report['loops'][0]['equations']))

    def test_conflicting_backedge_steps_do_not_prove_a_stride(self):
        report = lift(rows('mov r4,r0', 'movs r1,#0', 'cmp r2,#0', 'beq func+0xc',
                           'adds r4,#8', 'b func+0x4', 'adds r4,#12', 'b func+0x4'))
        equation = next(e for e in report['loops'][0]['equations'] if e['register'] == 'r4')
        self.assertEqual(equation['status'], 'unresolved')
        self.assertIsNone(equation['stride'])

    def test_stack_spill_recovers_value_after_non_aliasing_call(self):
        report = lift(rows('push {r4,lr}', 'sub sp,#8', 'str r0,[sp,#4]', 'bl #0x08001234',
                           'ldr r4,[sp,#4]', 'ldrb r1,[r4,#7]'))
        access = report['memory'][-1]
        self.assertEqual(access['address']['text'], 'arg0 + 7')
        self.assertEqual(report['calls'][0]['stack_arguments'], [])
        self.assertEqual(report['calls'][0]['arguments'][0]['text'], 'arg0')

    def test_stack_escape_invalidates_forwarded_contents(self):
        report = lift(rows('sub sp,#8', 'str r1,[sp,#4]', 'mov r0,sp', 'bl #0x08001234',
                           'ldr r4,[sp,#4]', 'str r2,[r4]'))
        self.assertIn('load_u32(entry_sp - 4)', report['memory'][-1]['address']['text'])
        self.assertNotEqual(report['memory'][-1]['address']['text'], 'arg1')

    def test_call_stack_candidates_exclude_push_saves_and_old_call_stores(self):
        report = lift(rows('push {r4-r7,lr}', 'sub sp,#8', 'movs r1,#7', 'str r1,[sp]',
                           'bl #0x08001234', 'bl #0x08005678'))
        first, second = report['calls']
        self.assertEqual(len(first['stack_arguments']), 1)
        self.assertEqual(first['stack_arguments'][0]['value']['text'], '7')
        self.assertEqual(first['stack_arguments'][0]['store_offsets'], (6,))
        self.assertEqual(second['stack_arguments'], [])

    def test_partial_overwrite_does_not_forward_old_word(self):
        report = lift(rows('sub sp,#4', 'str r0,[sp]', 'strb r1,[sp,#1]', 'ldr r4,[sp]', 'ldrb r2,[r4]'))
        self.assertIn('load_u32', report['memory'][-1]['address']['text'])
        self.assertNotEqual(report['memory'][-1]['address']['text'], 'arg0')

    def test_signedness_mismatch_and_duplicate_pairing_are_visible(self):
        target = lift(rows('ldrh r1,[r0]', 'ldrh r2,[r0]'))
        current = lift(rows('ldrsh r4,[r0]', 'ldrh r5,[r0]'))
        comparison = compare(target, current)
        self.assertEqual(len(comparison['paired']), 1)
        self.assertTrue(comparison['paired'][0]['multiplicity_ambiguous'])
        self.assertEqual(len(comparison['target_only']), 1)
        self.assertEqual(len(comparison['current_only']), 1)

    def test_branch_domain_and_compare_operands(self):
        signed = lift(rows('adds r0,#16', 'cmp r0,#31', 'bgt func+0x8', 'bx lr', 'bx lr'))
        unsigned = lift(rows('adds r0,#16', 'cmp r0,#31', 'bhi func+0x8', 'bx lr', 'bx lr'))
        first, = signed['branches']
        second, = unsigned['branches']
        self.assertEqual(first['domain'], 'signed')
        self.assertEqual(second['domain'], 'unsigned')
        self.assertEqual(first['arguments'][0]['text'], 'arg0 + 0x10')
        self.assertEqual(first['arguments'][1]['text'], '0x1F')
        self.assertNotEqual(first['identity_key'], second['identity_key'])

    def test_flag_changes_and_calls_invalidate_compare(self):
        for instruction in ('adds r1,#1', 'movs r2,#0', 'bl #0x08001234'):
            report = lift(rows('cmp r0,#4', instruction, 'bge func+0x8', 'bx lr', 'bx lr'))
            branch, = report['branches']
            self.assertEqual(branch['status'], 'unknown-flags')
            self.assertIsNone(branch['identity_key'])
        report = lift(rows('cmp r0,#4', 'mov r4,r0', 'bge func+0x8', 'bx lr', 'bx lr'))
        self.assertEqual(report['branches'][0]['status'], 'known-compare')

    def test_branch_joind_compare_is_not_invented(self):
        report = lift(rows('cmp r0,#0', 'beq func+0x8', 'cmp r1,#2', 'b func+0xa',
                           'cmp r1,#4', 'bge func+0xe', 'bx lr', 'bx lr'))
        self.assertEqual(report['branches'][-1]['status'], 'unknown-flags')

    def test_register_shift_uses_low_byte_of_amount(self):
        report = lift(rows('movs r0,#8', 'ldr r1,=257', 'lsrs r0,r1', 'str r0,[r2]'))
        self.assertEqual(report['memory'][0]['value']['text'], '4')

    def test_literal_pool_and_unknown_instruction(self):
        report = lift([(0,2,'ldr r4,[pc,#4]'), (2,2,'ldrb r1,[r4]'), (8,4,'.word 0x02001234')], entry_address=0x08000000)
        self.assertEqual(report['memory'][-1]['address']['text'], '0x2001234')
        unresolved = lift([(0,2,'ldr r4,[pc,#4]'), (2,2,'ldrb r1,[r4]'), (8,4,'.word 0x02001234')])
        self.assertIsNone(unresolved['memory'][-1]['identity_key'])
        report = lift(rows('mystery r0,r1', 'ldr r2,[r0]'))
        self.assertEqual(report['summary']['unsupported'], 1)
        self.assertIsNone(report['memory'][-1]['identity_key'])


if __name__ == '__main__':
    unittest.main()
