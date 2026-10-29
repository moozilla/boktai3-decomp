"""Reject compiler knob syntax that silently becomes an atoi-style partial no-op."""
import unittest
from search_eval import validate_knobs


class SearchKnobValidationTests(unittest.TestCase):
    def test_supported_named_and_numeric_formats(self):
        for knobs in (
            {'AGBCC_PRI': 'variant:+100,159:-1'},
            {'AGBCC_FORCE': 'actor:7,143:6'},
            {'CSE_NOCANON': 'clamped'},
            {'CSE_KEEP': 'clamped,143,variant'},
            {'AGBCC_NOPRE': '3'},
            {'AGBCC_NOMOVE': '42,98'},
            {'AGBCC_SPILL': '44:1,51:-2'},
            {'AGBCC_SPILL_LABEL': 'reg22#1:2,=0#3:-1'},
            {'CSE_NOFOLLOW_JUMP': '545:1,851:2,902'},
        ):
            with self.subTest(knobs=knobs):
                validate_knobs(knobs)

    def test_unknown_knob_rejected(self):
        with self.assertRaisesRegex(ValueError, 'unknown'):
            validate_knobs({'AGBCC_PR1': 'variant:+100'})

    def test_numeric_event_names_rejected_instead_of_atoi_zero(self):
        for knob in ('AGBCC_NOPRE', 'CSE_NOFOLLOW', 'AGBCC_NOTIE'):
            with self.subTest(knob=knob), self.assertRaises(ValueError):
                validate_knobs({knob: 'clamped'})

    def test_scalar_nopre_does_not_accept_silently_ignored_second_event(self):
        # gcse.c pre_delete compares atoi(nopre) with the event counter.
        with self.assertRaises(ValueError):
            validate_knobs({'AGBCC_NOPRE': '1,2'})

    def test_spill_label_requires_recorded_occurrence(self):
        # reload1.c agbcc_label_shift requires a final #occurrence selector.
        with self.assertRaises(ValueError):
            validate_knobs({'AGBCC_SPILL_LABEL': 'reg22:1'})

    def test_malformed_values_rejected(self):
        for knobs in ({'CSE_NOCANON': '22,143'}, {'AGBCC_SPILL': '44'},
                      {'AGBCC_PRI': 'variant:+1junk'}, {'CSE_NOFOLLOW_JUMP': '545:3'}):
            with self.subTest(knobs=knobs), self.assertRaises(ValueError):
                validate_knobs(knobs)


if __name__ == '__main__':
    unittest.main()
