"""scope try: a whole-file variant that replaces a private static helper is not taken for a snippet."""
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools" / "scope"))

import scope  # noqa: E402

BASE = """#include "global.h"
struct S { int a; };
inline static int Step(struct S *s) { return s->a; }
static inline int Other(struct S *s) { return s->a + 1; }
int Keep(struct S *s) { return s->a; }
int F(struct S *s) { return Step(s) + Other(s) + Keep(s); }
""".splitlines()


class Snippet(unittest.TestCase):
    def test_dropping_static_helpers_is_a_whole_file(self):
        text = ('#include "global.h"\n#include "shared.h"\nstruct S { int a; };\n'
                'int Keep(struct S *s) { return s->a; }\nint F(struct S *s) { return Shared(s); }\n')
        self.assertFalse(scope.is_snippet(BASE, text, "F"))

    def test_dropping_a_global_function_is_a_snippet(self):
        text = '#include "global.h"\nstruct S { int a; };\nint F(struct S *s) { return s->a; }\n'
        self.assertTrue(scope.is_snippet(BASE, text, "F"))

    def test_no_includes_is_a_snippet(self):
        self.assertTrue(scope.is_snippet(BASE, "int F(struct S *s) { return 0; }\n", "F"))

    def test_put_takes_the_function_the_snippet_defines(self):
        import tempfile
        with tempfile.TemporaryDirectory() as d:
            unit, snip = Path(d) / "u.c", Path(d) / "s.c"
            unit.write_text("\n".join(BASE) + "\n")
            snip.write_text("static inline int H(struct S *s) { return 2; }\n"
                            "int Keep(struct S *s)\n{\n    return H(s);\n}\n")
            self.assertEqual(scope.snippet_function(str(unit), str(snip)), "Keep")
            snip.write_text("int Missing(struct S *s) { return 0; }\n")
            self.assertIsNone(scope.snippet_function(str(unit), str(snip)))


if __name__ == "__main__":
    unittest.main()
