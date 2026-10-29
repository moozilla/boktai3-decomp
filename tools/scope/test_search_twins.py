import tempfile
from pathlib import Path
import unittest
from search_twins import find, source_location


class TwinTests(unittest.TestCase):
    def test_only_currently_matched_prior_art_is_returned(self):
        tokens = ['a', 'b', 'c', 'd']
        index = {'target': tokens, 'exact': tokens, 'draft': tokens, 'partial': ['x', *tokens, 'y']}
        status = {n: 'matched' for n in index}
        status['draft'] = 'nonmatching'
        rows = find(tokens, index, status, {n: n+'.c' for n in index}, 'target')
        self.assertEqual([m['function'] for m in rows], ['exact', 'partial'])
        self.assertEqual(rows[1]['matched_instruction'], 1)
        self.assertEqual(rows[0]['consecutive_instructions'], 4)

    def test_no_short_or_unrelated_matches(self):
        self.assertEqual(find(['a', 'b'], {}, {}, {}, 'target'), [])
        self.assertEqual(find(['a', 'b', 'c', 'd'], {'X': ['z']}, {'X': 'matched'}, {'X': 'x.c'}, 'target'), [])

    def test_alias_definition_location_is_a_function_anchor(self):
        with tempfile.TemporaryDirectory() as directory:
            Path(directory, 'x.c').write_text('#define Friendly sub_08001234\nvoid Friendly(int a)\n{\n}\n')
            location = source_location(directory, {'source': 'x.c', 'function': 'sub_08001234'})
            self.assertEqual(location, {'line': 2, 'name': 'Friendly'})


if __name__ == '__main__':
    unittest.main()
