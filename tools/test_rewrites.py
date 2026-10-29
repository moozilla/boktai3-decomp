"""Host-executed regression checks for semantics-preserving rewrite families."""
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

import permute
sys.path.insert(0, permute.PERMUTER)
from perm_pycparser import c_parser
import rewrites as R


class CopyVariableTests(unittest.TestCase):
    def variants(self, source):
        fn = c_parser.CParser().parse(source).ext[0]
        return [R.text(f) for _, f in R.rw_copy_var(fn)]

    def test_redefinitions_preserve_results(self):
        compiler = shutil.which("cc")
        if not compiler:
            self.skipTest("host C compiler unavailable")
        cases = [
            "int f(int x) { int v; int y; v=x; y=v; v=x+5; return y+v; }",
            "int f(int x) { int v; int y; v=x; y=v; if(x>0) v=7; return y+v; }",
            "int f(int x) { int v; int y; v=x; y=v; v++; return y+v; }",
            "int f(int x) { int v; int y; v=x; y=v; while(v<4) v++; return y+v; }",
            "int f(int x) { int v; int y; v=x; y=v; v+=3; return y+v; }",
        ]
        definitions, checks = [], []
        for case_id, source in enumerate(cases):
            variants = self.variants(source)
            self.assertTrue(variants, "safe copies before the redefinition should remain available")
            original = f"original{case_id}"
            definitions.append(source.replace("int f(", f"int {original}("))
            for variant_id, variant in enumerate(variants):
                name = f"variant{case_id}_{variant_id}"
                definitions.append(variant.replace("int f(", f"int {name}("))
                checks.append(f"if ({name}(x) != {original}(x)) return 1;")
        code = "\n".join(definitions) + "\nint main(void) { int x; for(x=-4;x<=8;x++) {\n"
        code += "\n".join(checks) + "\n} return 0; }\n"
        with tempfile.TemporaryDirectory() as tmp:
            src, exe = Path(tmp) / "copies.c", Path(tmp) / "copies"
            src.write_text(code)
            subprocess.run([compiler, "-std=gnu99", "-O0", str(src), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

    def test_address_taken_or_volatile_value_is_not_snapshotted(self):
        cases = [
            "int f(int x) { int v; v=x; mutate(&v); return v; }",
            "int f(int x) { volatile int v; v=x; return v; }",
        ]
        for source in cases:
            self.assertEqual(self.variants(source), [])


if __name__ == "__main__":
    unittest.main()
