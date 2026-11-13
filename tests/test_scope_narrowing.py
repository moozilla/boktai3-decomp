"""Do not mistake a removed byte conversion for a register allocation failure."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope


class RemovedNarrowing(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def pair(self, shift=24):
        return [(265, None, '(set (reg:SI 104) (ashift:SI (subreg:SI (reg:QI 124) 0) (const_int %d)))' % shift),
                (266, None, '(set (reg:SI 103) (lshiftrt:SI (reg:SI 104) (const_int %d)))' % shift)]

    def test_deleted_byte_conversion(self):
        self.assertEqual(self.scope.removed_narrowing(self.pair(), set(), [103]),
                         [(124, 8, 265, 266)])

    def test_deleted_halfword_conversion(self):
        self.assertEqual(self.scope.removed_narrowing(self.pair(16), set(), [103]),
                         [(124, 16, 265, 266)])

    def test_surviving_shift_is_not_a_removed_conversion(self):
        for survivor in (265, 266):
            self.assertEqual(self.scope.removed_narrowing(self.pair(), {survivor}, [103]), [])

    def test_other_shifts_and_duplicate_definitions_are_not_guessed(self):
        self.assertEqual(self.scope.removed_narrowing(self.pair(8), set(), [103]), [])
        pair = self.pair()
        pair.append((267, None, pair[0][2]))
        self.assertEqual(self.scope.removed_narrowing(pair, set(), [103]), [])

    def test_arithmetic_shift_is_not_unsigned_narrowing(self):
        pair = self.pair()
        pair[1] = (266, None, pair[1][2].replace('lshiftrt', 'ashiftrt'))
        self.assertEqual(self.scope.removed_narrowing(pair, set(), [103]), [])


if __name__ == '__main__':
    unittest.main()
