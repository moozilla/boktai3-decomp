"""ABI reductions must never be inferred from calls or unknown signatures."""

from __future__ import annotations

import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from matchsearch.contracts import infer_call_contracts


class PrototypeContracts(unittest.TestCase):
    def infer(self, source, **symbols):
        result = infer_call_contracts(source, symbols)
        self.assertEqual(result["warnings"], [])
        json.dumps(result)
        return result

    def test_explicit_zero_through_stack_arguments(self):
        result = self.infer(
            "void zero(void); int one(int); void many(int,int,int,int,int);",
            zero=(0x08000001, "FUNC"), one=0x08000002, many=0x08000004)
        self.assertEqual(result["arities"], {"#0x8000000": 0, "#0x8000002": 1, "#0x8000004": 4})
        self.assertEqual(result["provenance"]["#0x8000004"]["declarations"][0]["parameter_count"], 5)

    def test_typedefs_pointer_and_adjusted_parameter_types(self):
        result = self.infer(
            "typedef unsigned int u32; typedef u32 Count; typedef void (*Callback)(int);"
            "struct Big { long long data; };"
            "void f(const struct Big *, Count, Callback);"
            "void g(int data[20], void callback(int));"
            "typedef int Function(void); Function h;",
            f=0x8000100, g=0x8000200, h=0x8000300)
        self.assertEqual(result["arities"], {"#0x8000100": 3, "#0x8000200": 2, "#0x8000300": 0})

    def test_calls_and_pointer_objects_are_not_prototypes(self):
        result = self.infer(
            "int (*fp)(void); int target(void) { unknown(); fp(); return 0; }",
            fp=(0x8000100, "FUNC"), unknown=0x8000200, target=0x8000300)
        self.assertEqual(result["arities"], {"#0x8000300": 0})

    def test_oldstyle_and_variadic_remain_unknown(self):
        result = self.infer(
            "int a(); int b(int,...); int c(x) int x; { return x; } int d() { return 0; }",
            a=0x8000000, b=0x8000002, c=0x8000004, d=0x8000006)
        self.assertEqual(result["arities"], {})
        self.assertEqual(len(result["unknown"]), 4)

    def test_multiword_aggregate_and_hidden_return_are_unknown(self):
        result = self.infer(
            "typedef unsigned long long u64; struct S { int a,b; };"
            "void a(u64); void b(struct S); struct S c(void); void d(double);"
            "void e(long value); enum E { X }; void f(enum E);",
            a=0x8000000, b=0x8000002, c=0x8000004, d=0x8000006, e=0x8000008, f=0x800000a)
        self.assertEqual(result["arities"], {"#0x8000008": 1, "#0x800000a": 1})
        self.assertEqual(len(result["unknown"]), 4)

    def test_alias_conflicts_and_unknown_veto(self):
        result = self.infer(
            "void a(int); void b(void); void c(int); void d();"
            "void e(int,int,int,int); void f(int,int,int,int,int);",
            a=0x8000000, b=0x8000001, c=0x8000002, d=0x8000003, e=0x8000004, f=0x8000005)
        self.assertEqual(result["arities"], {})
        self.assertEqual(len(result["unknown"]), 3)

    def test_consistent_declarations_have_provenance(self):
        result = self.infer(
            '# 8 "api.h"\nvoid a(int);\n# 20 "file.c"\nvoid a(int x) { }\nvoid alias(int);',
            a=0x8000001, alias=0x8000000)
        entry = result["provenance"]["#0x8000000"]
        self.assertEqual(result["arities"], {"#0x8000000": 1})
        self.assertEqual(entry["symbols"], ["a", "alias"])
        self.assertEqual(len(entry["declarations"]), 3)
        self.assertTrue(entry["declarations"][0]["location"].startswith("api.h:8"))

    def test_static_and_nonfunction_symbols_cannot_reduce(self):
        result = self.infer("static void f(void) {} void g(void);", f=0x8000000, g=(0x8000002, "OBJECT"))
        self.assertEqual(result["arities"], {})
        self.assertEqual(len(result["unknown"]), 2)

    def test_comments_include_asm_and_string_calls(self):
        result = self.infer(
            'asm(".include \\"asm/macros.inc\\"");\n'
            '/* void fake(void); */ const char *s = "void fake(void);";\n'
            'void real(void); // fake();\n', real=0x8000000, fake=0x8000002)
        self.assertEqual(result["arities"], {"#0x8000000": 0})

    def test_parse_failure_is_explicit_and_conservative(self):
        result = infer_call_contracts("this is not preprocessed C", {"is": 0x8000000})
        self.assertEqual(result["arities"], {})
        self.assertIn("all calls retain unknown arity", result["warnings"][0])

    def test_gnu_type_attributes_and_assembler_names_are_unknown(self):
        result = infer_call_contracts(
            'typedef int Wide __attribute__((mode(DI))); void f(Wide);'
            'void g(void) asm("renamed"); void h(void) __attribute__((noreturn));',
            {"f": 0x8000000, "g": 0x8000002, "h": 0x8000004})
        self.assertEqual(result["arities"], {})
        if result.get("parser") == "perm_pycparser":
            self.assertEqual(result["warnings"], [])
            self.assertEqual(len(result["unknown"]), 3)

    def test_attributed_function_typedef_vetoes_alias_reduction(self):
        result = infer_call_contracts(
            'typedef void Fn(void) __attribute__((noreturn)); Fn f; void alias(void);',
            {"f": 0x8000001, "alias": 0x8000000})
        self.assertEqual(result["arities"], {})
        if result.get("parser") == "perm_pycparser":
            self.assertEqual(result["warnings"], [])
            self.assertEqual(len(result["unknown"]), 1)


if __name__ == "__main__":
    unittest.main()
