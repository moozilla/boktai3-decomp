"""Regression for loop.c's nonconstant induction multipliers."""
import unittest
import scope


class GivRecordsTests(unittest.TestCase):
    def test_constant_and_rtl_multipliers_across_scans(self):
        dump = '''Loop from 169 to 812: 188 real insns.
Insn 679: giv reg 360 src reg 26 benefit 18 lifetime 2 replaceable ncav mult (reg/v:SI 34) add 0
Insn 795: giv reg 409 src reg 33 benefit 2 lifetime 1 replaceable mult 1 add 136
giv at 679 reduced to (reg:SI 421)
Loop from 169 to 812: 169 real insns.
Insn 795: giv reg 409 src reg 33 benefit 2 lifetime 1 replaceable mult 1 add (reg/v:SI 9)
'''
        records = scope.giv_records(dump)
        self.assertEqual([r['uid'] for r in records], [679, 795, 795])
        self.assertEqual(records[0]['mult'], '(reg/v:SI 34)')
        self.assertEqual(records[0]['benefit'], 18)
        self.assertEqual(records[0]['life'], 2)
        self.assertEqual(records[0]['biv'], '26')
        self.assertEqual(records[2]['add'], '(reg/v:SI 9)')
        self.assertTrue(records[0]['repl'])

    def test_addresses_and_nested_rtl_are_not_discarded(self):
        dump = 'Insn 20: dest address src reg 4 benefit -2 lifetime 3 mult (const_int 16) add (plus:SI (reg:SI 2)\n    (const_int 4))'
        record, = scope.giv_records(dump)
        self.assertEqual(record['kind'], 'address')
        self.assertEqual(record['mult'], '(const_int 16)')
        self.assertEqual(record['add'], '(plus:SI (reg:SI 2) (const_int 4))')
        self.assertFalse(record['repl'])


if __name__ == '__main__':
    unittest.main()
