"""Equivalent flag spellings should charge the same number of source uses."""
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[1] / "tools"
sys.path.insert(0, str(TOOLS))
import concessions as c
import progress


class FlagDebt(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name)
        (self.root / "include").mkdir()
        (self.root / "src").mkdir()
        (self.root / "include/flags.h").write_text(
            "static inline unsigned Test(unsigned *f, unsigned mask) { return *f & mask; }\n")
        (self.root / "include/actor.h").write_text('#include "flags.h"\n')

    def debt(self, source):
        sites = c.sites(source, "src/a.c") + c.literal_argument_sites("src/a.c", source, self.root)
        return sum(s.weight for s in c.warnings(sites) if s.kind in c.STAGED)

    def test_each_use_counts_even_with_dynamic_call_and_nested_header(self):
        base = '#include "actor.h"\nint f(unsigned *p, unsigned x) { Test(p, x);\n'
        for decl in ("unsigned m = 4;", "unsigned m; m = FLAG_A | FLAG_B;"):
            parked = base + decl + "\nreturn (*p & m) + Test(p, m); }"
            parked = parked.replace("\nunsigned m", "\n    unsigned m")
            self.assertEqual(self.debt(parked), 2)
        self.assertEqual(self.debt(base + "return Test(p, 4) + Test(p, 4); }"), 2)

    def test_multiple_bindings_and_parameters_on_one_line(self):
        self.assertEqual(self.debt("int f(int x) { int a = 4, b = 2; return (x & a) | b; }"), 2)
        code = "static inline int Both(int x, int m, int n) { return (x & m) | n; }\n"
        self.assertEqual(self.debt(code + "int f(int x) { return Both(x, 4, 2); }"), 2)

    def test_evidence_follows_operation_and_owner(self):
        pause = ("static inline unsigned Check(unsigned m) { return (gEngineFlags | gScenePauseFlags) & m; }\n"
                 "unsigned f(void) { return Check(4); }")
        self.assertEqual(self.debt(pause), 0)
        self.assertEqual(self.debt(pause.replace("gScenePauseFlags", "gOtherFlags")), 1)
        tracking = ("static inline int Test(struct SceneActorChaseTracking *p, unsigned mask) "
                    "{return p->flags & mask;}\nint f(void) {return Test(p, 8);}")
        self.assertEqual(self.debt(tracking), 0)
        self.assertEqual(self.debt(tracking.replace("SceneActorChaseTracking", "Other")), 1)

    def test_separately_assigned_number_name_counts_repeated_uses(self):
        self.assertEqual(self.debt("int f(int x) { int one; one = 1; return (x & one) | one; }"), 2)

    def test_header_definition_evidence_and_clone(self):
        header = self.root / "include/flags.h"
        header.write_text("// concession-ok(literal-param): the SDK header defines this operation\n" + header.read_text())
        call = "unsigned f(unsigned *p) { return Test(p, 4); }"
        self.assertEqual(self.debt('#include "flags.h"\n' + call), 0)
        local = "static inline unsigned Test(unsigned *p, unsigned m) { return *p & m; }\n"
        self.assertEqual(self.debt('#include "flags.h"\n' + local + call), 1)
        self.assertEqual(self.debt(call), 0)

    def test_progress_counts_weighted_uses(self):
        lines = progress.concession_lines_md({"literal-local": (2, 0), "literal-param": (3, 0)})
        self.assertTrue(any("Steering debt" in line and "**5**" in line for line in lines))

    def test_no_annotations_skip_second_source_scan(self):
        from unittest.mock import patch
        with patch.object(c, "sites", side_effect=AssertionError("redundant scan")):
            self.assertEqual(c.lint("int f(void) { return 1; }"), [])
        self.assertTrue(c.lint("// concession-ok(made-up): a meaningful reason"))

    def test_bulk_lookup_reuses_header_facts_but_land_sees_edits(self):
        from unittest.mock import patch
        call = '#include "actor.h"\nunsigned f(unsigned *p) { return Test(p, 1); }'
        headers = {}
        c.literal_argument_sites("src/a.c", call, self.root, headers=headers)
        with patch.object(c, "_header_facts", side_effect=AssertionError("duplicate header lookup")):
            self.assertEqual(len(c.literal_argument_sites("src/b.c", call, self.root, headers=headers)), 1)
        (self.root / "include/flags.h").write_text("static inline int Test(unsigned *p, unsigned m) { return *p + m; }")
        self.assertEqual(c.literal_argument_sites("src/b.c", call, self.root), [])

    def test_filtered_cli_skips_global_macro_inventory(self):
        import contextlib
        import io
        from unittest.mock import patch
        source = self.root / "src/a.c"
        source.write_text("int f(int x) { int m = 4; return x & m; }")
        stderr = io.StringIO()
        with contextlib.redirect_stderr(stderr), patch("statement_macros.single_use_names",
                                                      side_effect=AssertionError("unrelated corpus scan")):
            self.assertEqual(c.main(["--no-compiler", "--kind", "literal-local", "--summary", str(source)]), 0)
        self.assertIn("1 warnings in 1 files", stderr.getvalue())

    def test_macro_temporary_and_plain_flag_macro(self):
        use = "\nint f(int *p) { return Test(p, 4); }"
        self.assertEqual(self.debt("#define Test(p,k) ((*(p)) & (k))" + use), 0)
        temp = "#define Test(p,k) ({ unsigned m = (k); \\\n (*(p)) & m; })"
        self.assertEqual(self.debt(temp + use), 1)
        self.assertEqual(self.debt(temp.replace("k", "K") + use), 1)
        self.assertEqual(self.debt(temp.replace("& m", "+ m") + use), 0)

    def test_summary_and_total_include_the_same_weighted_uses(self):
        import contextlib
        import io
        from unittest.mock import patch
        source = self.root / "src/a.c"
        source.write_text("static inline unsigned Test(unsigned x, unsigned m) { return x & m; }\n"
                          "unsigned f(unsigned x) { unsigned m = 4; return Test(x, 2) + (x & m) + (x | m); }")
        output = io.StringIO()
        with contextlib.redirect_stderr(output), patch("statement_macros.single_use_names", return_value=frozenset()):
            self.assertEqual(c.main(["--no-compiler", "--summary", str(source)]), 0)
        self.assertIn("steering debt: 3 constant uses", output.getvalue())
        self.assertIn("0 errors, 3 warnings in 1 files", output.getvalue())
