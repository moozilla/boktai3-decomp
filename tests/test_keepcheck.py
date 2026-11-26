"""tools/keepcheck.py: the keep-or-revert gate the cleanup tools share."""
import os
import sys
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import keepcheck  # noqa: E402

DRAFTED = """\
int a(void)
{
    return 1;
}
#ifdef NONMATCHING
int b(void)
{
    return 2;
}
#else
INCLUDE_ASM("asm/nonmatching", b);
#endif
"""


class Helpers(unittest.TestCase):
    def test_draft_ranges_cover_the_ifdef_branch_only(self):
        (a, b), = keepcheck.draft_ranges(DRAFTED)
        self.assertIn("return 2", DRAFTED[a:b])
        self.assertNotIn("INCLUDE_ASM", DRAFTED[a:b])
        self.assertFalse(keepcheck.in_ranges(DRAFTED.index("return 1"), [(a, b)]))

    def test_function_spans(self):
        spans = keepcheck.function_spans(DRAFTED)
        self.assertEqual([n for _, _, n in spans], ["a", "b"])
        a0, a1, _ = spans[0]
        self.assertTrue(DRAFTED[a0:a1].rstrip().endswith("}"))

    def test_blank_comments_keeps_offsets(self):
        text = 'x = 1; /* y = 2; */ s = "z = 3"; // w\n'
        out = keepcheck.blank_comments(text)
        self.assertEqual(len(out), len(text))
        self.assertNotIn("y", out)
        self.assertNotIn("z", out)

    def test_src_files_skips_lib_and_the_skip_list(self):
        files = keepcheck.src_files(["src"], skip={"src/system/clock/sun_clock.c"})
        self.assertNotIn("src/system/clock/sun_clock.c", files)
        self.assertFalse(any(keepcheck.is_upstream(f) for f in files))


@unittest.skipUnless(os.path.exists(os.path.join(ROOT, "build", "boktai3.elf"))
                     and os.path.exists(os.path.join(ROOT, "baserom.gba")), "needs a built tree")
class Gate(unittest.TestCase):
    REL = "src/system/clock/sun_clock.c"

    def setUp(self):
        self.path = os.path.join(ROOT, self.REL)
        self.orig = open(self.path).read()

    def tearDown(self):
        with open(self.path, "w") as f:
            f.write(self.orig)

    def test_mismatch_is_reverted(self):
        u = keepcheck.Unit(self.REL)
        self.assertFalse(u.attempt(self.orig.replace("c->hourA > 23", "c->hourA > 22", 1)))
        self.assertEqual(open(self.path).read(), self.orig)

    def test_new_warning_is_reverted(self):
        u = keepcheck.Unit(self.REL)
        bad = self.orig + "\nint *__kc_p = (short *)0;\n"  # data, not code: only the warning differs
        self.assertFalse(u.attempt(bad))
        self.assertIn("warning", u.last)
        self.assertEqual(open(self.path).read(), self.orig)

    def test_neutral_change_is_kept(self):
        u = keepcheck.Unit(self.REL)
        self.assertTrue(u.attempt(self.orig + "\n/* neutral */\n"))
        self.assertTrue(open(self.path).read().endswith("/* neutral */\n"))


if __name__ == "__main__":
    unittest.main()
