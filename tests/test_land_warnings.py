"""Land compares compiler warning counts only when both versions compile."""

import os
import sys
import tempfile
import unittest
from unittest.mock import patch

TOOLS = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "tools")
sys.path.insert(0, os.path.join(TOOLS, "scope"))
sys.path.insert(0, TOOLS)
import deadcode  # noqa: E402
import land  # noqa: E402


class LandWarnings(unittest.TestCase):
    def setUp(self):
        self.path = os.path.join(os.path.dirname(TOOLS), "src", "test.c")
        self.before = "void f(void) {}\n"
        self.after = "void f(void) {\n    int zero = 0;\n}\n"
        self.compiler_warning = ("proved-zero", "test.c", 2, "f", "shift")

    def test_unavailable_baseline_keeps_the_static_warning_delta(self):
        with tempfile.TemporaryDirectory() as cache, patch.object(deadcode, "CACHE", cache), \
                patch.object(deadcode, "available", return_value=True), \
                patch.object(deadcode, "compile_unit", side_effect=RuntimeError("changed header")):
            self.assertEqual(land.added_warnings(self.before, self.after, self.path),
                             ({"const-local": 1}, "previous"))

    def test_literal_moved_into_an_inline_parameter_is_an_added_warning(self):
        helper = ("static inline u32 TestLandFlags(u32 *flags, u32 mask)\n{\n    return *flags & mask;\n}\n"
                  "u32 f(u32 *p) {\n    return TestLandFlags(p, 1);\n}\n")
        with patch.object(deadcode, "available", return_value=False):
            self.assertEqual(land.added_warnings("u32 f(u32 *p) {\n    return *p & 1;\n}\n", helper, self.path),
                             ({"literal-param": 1}, None))
            self.assertEqual(land.added_warnings(helper, helper, self.path), ({}, None))

    def test_existing_header_cannot_hide_a_transfer_in_either_direction(self):
        head = '#include "word_flags.h"\nu32 f(u32 *p) {\n'
        local = head + "    u32 mask = 4;\n    return *p & mask;\n}\n"
        call = head + "    return WordFlags_Test(p, 4);\n}\n"
        with patch.object(deadcode, "available", return_value=False):
            for before, after in ((local, call), (call, local)):
                added, error = land.added_warnings(before, after, self.path)
                self.assertEqual(added["possible local/argument transfers"], 1)
                self.assertIsNone(error)

    def test_compiler_warning_delta_uses_both_successful_compiles(self):
        with tempfile.TemporaryDirectory() as cache, patch.object(deadcode, "CACHE", cache), \
                patch.object(deadcode, "available", return_value=True), \
                patch.object(deadcode, "compile_unit", side_effect=[[], [self.compiler_warning]]):
            self.assertEqual(land.added_warnings(self.before, self.after, self.path),
                             ({"const-local": 1, "proved-zero": 1}, None))
