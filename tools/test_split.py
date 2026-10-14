"""Regression checks for reviewed incbin boundaries and guarded fallbacks."""
import unittest
from split import c_replacement_end


class ReplacementEndTests(unittest.TestCase):
    def test_hidden_function_fallback_consumes_its_original_blob(self):
        code = ['thumb_func_start sub_08044934', 'sub_08044934:',
                '\t.incbin "baserom.gba", 0x44934, 0x1a0']
        self.assertEqual(c_replacement_end(code, 0, 3, includes_asm=True), 3)

    def test_exact_body_keeps_unreviewed_trailing_blob(self):
        code = ['thumb_func_start sub_08044854', 'sub_08044854:',
                '\tbx lr', '_08044934:',
                '\t.incbin "baserom.gba", 0x44934, 0x34c']
        self.assertEqual(c_replacement_end(code, 0, 5), 3)

    def test_reviewed_exact_blob_is_consumed(self):
        code = ['thumb_func_start sub_08044934', 'sub_08044934:',
                '\t.incbin "baserom.gba", 0x44934, 0x1a0']
        self.assertEqual(c_replacement_end(code, 0, 3,
                         covered_ranges=[(0x08044934, 0x08044AD4)]), 3)


if __name__ == '__main__':
    unittest.main()
