"""Quantity arithmetic regression cases prompted by two still-unmatched drafts."""
from pathlib import Path
import tempfile
import unittest

from search_priority import analyze, priority, quantities, threshold


def quantity(number, refs, lifetime, words=1, block=1, birth=0, name=None):
    return {"quantity": number, "pseudo": number + 20, "refs": refs,
            "lifetime_half_insns": lifetime, "priority": priority(refs, lifetime, words),
            "register_words": words, "block": block, "birth": birth,
            "death": birth + lifetime, "suggestions": 0, "name": name}


class PriorityTests(unittest.TestCase):
    def test_actual_formula_boundary(self):
        self.assertEqual(priority(4, 98), 816)
        self.assertEqual(priority(4, 96), 833)
        self.assertEqual(priority(3, 36), 833)
        self.assertEqual(priority(4, 95), 842)

    def test_quantity_tie_boundary(self):
        table, constant = quantity(2, 4, 98), quantity(5, 3, 36)
        result = threshold(table, constant, 8)
        self.assertEqual(result["required_priority"], 833)
        self.assertEqual(result["priority_delta"], 17)
        self.assertEqual(result["selected_shorten_half_insns"], 2)
        self.assertEqual(result["selected_add_refs"], 1)
        self.assertEqual(result["competitor_lengthen_half_insns"], 1)
        self.assertEqual(result["competitor_remove_refs"], 1)

    def test_later_quantity_requires_strict_advantage(self):
        result = threshold(quantity(5, 4, 98), quantity(2, 3, 36), 8)
        self.assertFalse(result["equal_priority_wins"])
        self.assertEqual(result["required_priority"], 834)
        self.assertEqual(result["selected_shorten_half_insns"], 3)

    def test_small_block_threshold_is_strict(self):
        result = threshold(quantity(0, 4, 98), quantity(1, 3, 36), 3)
        self.assertEqual(result["selected_shorten_half_insns"], 3)
        self.assertIn("not simulated", result["tie_rule"])

    def test_one_reference_cannot_improve_by_shortening(self):
        result = threshold(quantity(0, 1, 20), quantity(1, 2, 20), 5)
        self.assertIsNone(result["selected_shorten_half_insns"])
        self.assertEqual(result["selected_add_refs"], 1)

    def test_already_above(self):
        result = threshold(quantity(0, 8, 10), quantity(1, 2, 20), 5)
        self.assertEqual(result["priority_delta"], 0)
        self.assertEqual(result["selected_shorten_half_insns"], 0)
        self.assertEqual(result["selected_add_refs"], 0)
        self.assertEqual(result["competitor_lengthen_half_insns"], 0)
        self.assertEqual(result["competitor_remove_refs"], 0)

    def test_refs_cross_floor_log_boundary(self):
        result = threshold(quantity(0, 3, 200), quantity(1, 4, 100), 8)
        self.assertEqual(result["selected_add_refs"], 5)
        self.assertLess(priority(7, 200), result["required_priority"])
        self.assertGreaterEqual(priority(8, 200), result["required_priority"])

    def test_parse_actual_quantity_coordinates_and_unallocated(self):
        text = "L Example 15 169 2 816 28 126 4 0 9 table\nL Example 15 230 5 833 70 106 3 0 -1 =24\n"
        result = quantities(text, "Example")
        self.assertEqual(result[0]["lifetime_half_insns"], 98)
        self.assertEqual(result[0]["register_words"], 1)
        self.assertEqual(result[1]["hard_register"], -1)

    def test_missing_size_is_not_invented(self):
        x = quantities("L Example 0 30 0 12345 0 10 2 0 0 value", "Example")[0]
        self.assertIsNone(x["register_words"])
        x["register_words"] = None
        self.assertIsNone(threshold(x, quantity(1, 2, 20), 5)["selected_add_refs"])

    def test_multiword_size_inferred_from_observed_formula(self):
        x = quantities("L Example 0 30 0 16000 0 10 4 0 0 pair", "Example")[0]
        self.assertEqual(x["register_words"], 2)

    def test_malformed_and_other_functions_ignored(self):
        self.assertEqual(quantities("L Other 0 30 0 2000 0 10 2 0 0 x\nL Example broken", "Example"), [])

    def test_query_preserves_ambiguity_and_overlap(self):
        class Scope:
            INSTR = "build/scope/agbcc/gcc/agbcc"
            def dump_func(self, wd, function):
                return function
            def pseudo_names(self, wd, function):
                return {30: "table", 31: "table", 32: "=24", 33: "disjoint", 34: "otherblock"}
        with tempfile.TemporaryDirectory() as directory:
            Path(directory, "names.txt").write_text(
                "L Example 0 30 0 816 28 126 4 0 9 table\n"
                "L Example 0 31 1 833 0 96 4 1 8 table\n"
                "L Example 0 32 2 833 70 106 3 0 8 =24\n"
                "L Example 0 33 3 2000 140 150 2 0 0 disjoint\n"
                "L Example 1 34 0 2000 20 30 2 0 0 otherblock\n")
            result = analyze(Scope(), directory, "Example", "table", "=24")
            self.assertTrue(result["ambiguous"])
            self.assertEqual(len(result["matches"]), 2)
            first = result["matches"][0]
            self.assertEqual(len(first["competitors"]), 1)
            self.assertEqual(first["competitors"][0]["overlap_half_insns"], [70, 106])
            self.assertFalse(first["competitors"][0]["suggestion_phase_may_precede_priority"])
            numeric = analyze(Scope(), directory, "Example", "30", "32")
            self.assertFalse(numeric["ambiguous"])
            self.assertEqual(numeric["matches"][0]["selected"]["pseudo"], 30)


if __name__ == "__main__":
    unittest.main()
