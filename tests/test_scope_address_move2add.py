"""Same-base move2add needs an intact physical address history, including labels."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class AddressMove2Add(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    lreg = [(10, 'insn', '(set (reg/v:SI 44) (plus:SI (reg/v:SI 22) (const_int 154)))'),
            (20, 'insn', '(set (reg/v:SI 46) (plus:SI (reg/v:SI 22) (const_int 52)))')]
    greg = [(9, 'insn', '(set (reg/v:SI 1 r1) (reg/v:SI 5 r5))'),
            (10, 'insn', '(set (reg/v:SI 1 r1) (plus:SI (reg/v:SI 1 r1) (const_int 154)))'),
            (11, 'insn', '(set (reg:HI 0 r0) (const_int 30))'),
            (12, 'insn', '(set (mem:HI (reg/v:SI 1 r1) 7) (reg:HI 0 r0))'),
            (20, 'insn', '(set (reg/v:SI 1 r1) (plus:SI (reg/v:SI 1 r1) (const_int -102)))')]
    homes = {22: 5, 44: 1, 46: 1}

    def detect(self, greg=None, lreg=None):
        return self.scope.move2add_address(self.lreg if lreg is None else lreg,
                                          self.greg if greg is None else greg, self.homes, 20)

    def test_reused_field_address(self):
        self.assertEqual(self.detect(), (1, 5, 154, 52, 10))

    def test_label_call_or_jump_blocks_history(self):
        for kind in ['code_label', 'call_insn', 'jump_insn', 'barrier']:
            with self.subTest(kind=kind):
                self.assertIsNone(self.detect(self.greg[:-1] + [(15, kind, '')] + self.greg[-1:]))

    def test_overwritten_base_or_destination_blocks_history(self):
        for reg in [1, 5]:
            for operation in ['set', 'clobber']:
                with self.subTest(reg=reg, operation=operation):
                    pat = f'({operation} (reg:SI {reg} r{reg})' + (' (const_int 0))' if operation == 'set' else ')')
                    self.assertIsNone(self.detect(self.greg[:-1] + [(15, 'insn', pat)] + self.greg[-1:]))

    def test_implicit_address_update_blocks_history(self):
        for op in ['pre_inc', 'post_inc', 'pre_dec', 'post_dec', 'pre_modify', 'post_modify']:
            with self.subTest(op=op):
                pat = f'(set (reg:SI 0 r0) (mem:SI ({op}:SI (reg:SI 1 r1))))'
                self.assertIsNone(self.detect(self.greg[:-1] + [(15, 'insn', pat)] + self.greg[-1:]))

    def test_same_delta_with_other_source_is_not_evidence(self):
        lreg = [(10, 'insn', self.lreg[0][2].replace('SI 22)', 'SI 23)')), self.lreg[1]]
        self.assertIsNone(self.detect(lreg=lreg))

    def test_copy_from_other_base_is_not_evidence(self):
        self.assertIsNone(self.detect([(9, 'insn', self.greg[0][2].replace('5 r5', '4 r4'))] + self.greg[1:]))

    def test_unchanged_offset_is_not_address_reuse(self):
        self.assertIsNone(self.detect(self.greg[:-1] + [(20, 'insn', self.greg[-1][2].replace('-102', '52'))]))


if __name__ == '__main__':
    unittest.main()
