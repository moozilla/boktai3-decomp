"""A do { ... } while (0) macro used at only one site counts as a barrier; a shared one is ordinary C."""
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "tools" / "scope"))

import statement_macros as sm  # noqa: E402
import land  # noqa: E402

ONCE = """#define BUMP(c) \\
    do { \\
        (*(c))++; \\
    } while (0)
void f(u8 *c) { BUMP(c); }
"""
SHARED_DEF = """#define DELTAS(a, b) \\
    do { \\
        a = b; \\
    } while (0)
"""


class StatementMacros(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        for sub in ("src/fn", "include", "src/lib"):
            (self.root / sub).mkdir(parents=True)
        (self.root / "src/fn/once.c").write_text(ONCE)
        (self.root / "include/deltas.h").write_text(SHARED_DEF)
        (self.root / "src/fn/a.c").write_text('#include "deltas.h"\nvoid a(void) { DELTAS(x, y); }\n')
        (self.root / "src/fn/b.c").write_text('#include "deltas.h"\nvoid b(void) { DELTAS(x, y); }\n')
        (self.root / "src/lib/lib.c").write_text("#define COPY(d, s) do { d = s; } while (0)\n"
                                                 "void l(void) { COPY(x, y); }\n")

    def tearDown(self):
        self.tmp.cleanup()

    def test_single_use_names(self):
        names = sm.single_use_names(self.root)
        self.assertIn("BUMP", names)
        self.assertNotIn("DELTAS", names)  # shared by two functions
        self.assertNotIn("COPY", names)  # library code is the original authors'

    def test_draft_replacing_its_own_file(self):
        path = self.root / "src/fn/once.c"
        self.assertEqual(sm.single_use_in(ONCE, exclude=path, root=self.root), ["BUMP"])
        self.assertEqual(sm.single_use_in('void c(void) { DELTAS(x, y); }\n', root=self.root), [])

    def test_only_do_while_macros_are_statement_macros(self):
        defs, _ = sm.scan("#define SQ(x) ((x) * (x))\n#define LOOP() do { } while (0)\n")
        self.assertEqual(defs, {"LOOP"})

    def test_a_second_site_makes_it_shared(self):
        got = sm.single_use_in(ONCE + "void g(u8 *c) { BUMP(c); }\n", root=self.root,
                               exclude=self.root / "src/fn/once.c")
        self.assertEqual(got, [])


class RepoMacros(unittest.TestCase):
    def test_land_flags_a_new_single_use_wrapper(self):
        draft = ("#define ZZ_TEST_WRAP(c) \\\n    do { \\\n        (*(c))++; \\\n    } while (0)\n"
                 "void zz(u8 *c) { ZZ_TEST_WRAP(c); }\n")
        text, pins, bars = land.concessions(draft)
        self.assertEqual((pins, bars), (0, 1))
        self.assertIn(land.MARKER, text)

    def test_evaluator_counts_it(self):
        from tools.matchsearch import evaluator
        got = evaluator._single_use_macros("#define ZZ_W(c) do { (c)++; } while (0)\nvoid z(int c) { ZZ_W(c); }\n",
                                           ROOT / "src/fn/zz_none.c", ROOT)
        self.assertEqual(got, ["ZZ_W"])

    def test_progress_marks_its_users(self):
        import progress
        self.assertIn("single-use-macro", progress.hint_kinds("void f(void) { %s(x); }" % sorted(
            __import__("statement_macros").single_use_names())[0]))


if __name__ == "__main__":
    unittest.main()
