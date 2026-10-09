import sys
from pathlib import Path
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
from benchmark_check import classify


class BenchmarkCheckTests(unittest.TestCase):
    def test_compile_error_cannot_count_as_match(self):
        self.assertEqual(classify(1, "agbcc failed: error\n"), {"status": "error"})
        result = classify(1, "sub_08000100: MATCH (12 bytes at 0x08000100)\n")
        self.assertNotEqual(result["status"], "match")

    def test_mismatch_is_not_match_substring(self):
        text = "sub_08000100: MISMATCH (12 bytes at 0x08000100)\n"
        self.assertEqual(classify(1, text)["status"], "mismatch")
        result = classify(0, text.replace("MISMATCH", "MATCH"))
        self.assertEqual(result["status"], "match")
        self.assertEqual(result["bytes"], 12)
