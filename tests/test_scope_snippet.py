"""scope try: a whole-file variant that replaces a private static helper is not taken for a snippet."""
from pathlib import Path
from contextlib import redirect_stdout
import io
import sys
import tempfile
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


class AliasReplacement(unittest.TestCase):
    func = "sub_08069788"

    def replace(self, base, *snippets):
        with tempfile.TemporaryDirectory() as directory:
            unit, snippet = Path(directory) / "unit.c", Path(directory) / "snippet.c"
            unit.write_text(base)
            for text in snippets:
                snippet.write_text(text)
                with redirect_stdout(io.StringIO()):
                    scope.put_definition(str(unit), str(snippet), self.func)
            return unit.read_text()

    def test_canonical_snippet_replaces_aliased_definition_and_prototype(self):
        text = self.replace(
            "#define Active sub_08069788\nint Active(void);\n"
            "int Keep(void) { return 7; }\n// Keep this comment.\n"
            "int Active(void) { return 0; }\n",
            "int sub_08069788(int value) { return value; }\n")
        self.assertIn("int sub_08069788(int value);", text)
        self.assertIn("int Keep(void) { return 7; }", text)
        self.assertIn("// Keep this comment.", text)
        self.assertNotIn("int Active(void)", text)
        self.assertEqual(text.count("{ return value; }"), 1)

    def test_aliased_snippet_keeps_earlier_prototype_canonical(self):
        text = self.replace(
            "int sub_08069788(void);\nint Keep(void) { return 7; }\n"
            "int sub_08069788(void) { return 0; }\n",
            "#define Active sub_08069788\nint Active(int value) { return value; }\n")
        self.assertIn("int sub_08069788(int value);", text)
        self.assertIn("int Active(int value) { return value; }", text)
        self.assertLess(text.index("int sub_08069788(int value);"), text.index("#define Active"))

    def test_repeated_alias_replacement_does_not_duplicate_helpers(self):
        text = self.replace(
            "int sub_08069788(void) { return 0; }\n",
            "#define Active sub_08069788\nstatic int Helper(void) { return 1; }\n"
            "int Active(void) { return Helper(); }\n",
            "#define Active sub_08069788\nstatic int Helper(void) { return 2; }\n"
            "int Active(void) { return Helper(); }\n")
        self.assertEqual(text.count("#define Active"), 1)
        self.assertEqual(text.count("static int Helper"), 1)
        self.assertEqual(text.count("int Active(void)"), 1)
        self.assertIn("return 2;", text)
        self.assertNotIn("return 1;", text)

    def test_multiline_pointer_return_uses_same_definition_matcher(self):
        text = self.replace(
            "#define Active sub_08069788\nvoid *\nActive(void) { return 0; }\n",
            "void *\nsub_08069788(void *value) { return value; }\n")
        self.assertIn("sub_08069788(void *value) { return value; }", text)
        self.assertNotIn("return 0;", text)


if __name__ == "__main__":
    unittest.main()
