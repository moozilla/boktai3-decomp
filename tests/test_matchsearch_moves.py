"""Regression tests for actual semantic hazards in the move catalog.

These are not a solved-ROM benchmark.  The small programs expose wrong rewrite
guards (aliasing, integer conversion, initializer order, and continue edges).
The final test compiles the original and emitted variants with an independent
host compiler and compares observable return values over boundary inputs.
"""

from __future__ import annotations

import ctypes
import importlib.util
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from matchsearch.moves import MoveCatalog, MoveError, Region


def catalog(source: str, **kwargs) -> MoveCatalog:
    return MoveCatalog.from_source(source, "f", repo_root=ROOT, **kwargs)


def moves(source: str, family: str):
    return list(catalog(source).moves([family]))


class MoveGuards(unittest.TestCase):
    def test_deterministic_and_bounded(self):
        source = "int f(int a, int b) { int x; int y; x=a+1; y=b+2; if(x&&y) return x; return y; }"
        first = catalog(source)
        a = list(first.moves())
        b = list(catalog(source).moves())
        self.assertEqual([m.id for m in a], [m.id for m in b])
        self.assertEqual(len(list(first.moves(limit=3))), 3)
        self.assertEqual(first.apply(a[0].id), a[0].source)
        self.assertTrue(all(m.proof_grade == "guarded" for m in a))
        self.assertGreater(len({m.family for m in first.moves(limit=5)}), 3)

    def test_statement_order_checks_dependencies_and_effects(self):
        independent = "int f(int a,int b){int x;int y;x=a+1;y=b+2;return x+y;}"
        dependent = "int f(int a,int b){int x;int y;x=a+1;y=x+2;return y;}"
        global_read = "int g; int f(int a,int b){int x;int y;x=g;y=b;return x+y;}"
        memory_read = "int f(int a,int *p){int x;int y;x=*p;y=a;return x+y;}"
        volatile = "int f(int a,int b){volatile int x;int y;x=a;y=b;return x+y;}"
        self.assertEqual(len(moves(independent, "statement_order")), 1)
        for source in (dependent, global_read, memory_read, volatile):
            self.assertEqual(moves(source, "statement_order"), [])

    def test_statement_order_offers_contact_style_constant_field_stores(self):
        source = ("typedef unsigned char u8; typedef unsigned short u16;"
                  "struct Slot { u8 enter,state; u16 stateFrame; u8 direction; };"
                  "void f(struct Slot *slot){slot->state=0;slot->stateFrame=0;"
                  "slot->enter=1;slot->direction=0;}")
        candidates = moves(source, "statement_order")
        self.assertEqual(len(candidates), 3)
        self.assertTrue(all(move.proof_grade == "guarded" for move in candidates))

    def test_field_store_order_rejects_dependencies_and_unknown_aliases(self):
        prefix = "struct Slot { int a,b; }; int side(void);"
        statements = [
            "p->a=1;p->a=2;",             # repeated destination
            "p->a=1;q->b=2;",             # no alias proof for another base
            "p->a=1;p->b=p->a;",          # dependent memory read
            "p->a=1;p->b=*value;",        # unknown memory read
            "p->a=1;p->b=side();",        # observable call
            "p->a=1;p->b=i++;",           # scalar side effect
            "p->a=1;p->b=i;",             # bounded policy: constants only
            "p->a=1;p=q;",                # receiver changes
            "p++->a=1;p->b=2;",           # effectful receiver
            "p[0].a=1;p[1].b=2;",         # no array/address-chain inference
            "p->a+=1;p->b=2;",            # no read/modify/write inference
        ]
        for body in statements:
            with self.subTest(body=body):
                source = prefix + "void f(struct Slot *p,struct Slot *q,int *value,int i){" + body + "}"
                self.assertEqual(moves(source, "statement_order"), [])

    def test_field_store_order_rejects_union_bitfields_and_local_tag_shadow(self):
        sources = [
            "union Slot{int a,b;};void f(union Slot*p){p->a=1;p->b=2;}",
            "struct Slot{unsigned a:1,b:1;};void f(struct Slot*p){p->a=1;p->b=0;}",
            "struct Slot{int a,b;};void f(void){struct Slot{volatile int a;int b;} s;"
            "struct Slot*p=&s;p->a=1;p->b=2;}",
            "struct Slot;void f(struct Slot*p){p->a=1;p->b=2;}",
        ]
        for source in sources:
            with self.subTest(source=source):
                self.assertEqual(moves(source, "statement_order"), [])

    def test_store_guards_follow_volatile_typedefs(self):
        sources = [
            "typedef volatile int V;typedef V Alias;"
            "int f(int a,int b){Alias x;int y;x=a;y=b;return x+y;}",
            "struct Slot{int a,b;};void f(volatile struct Slot*p){p->a=1;p->b=2;}",
            "struct Slot{int a,b;};typedef volatile struct Slot V;typedef V Alias;"
            "void f(Alias*p){p->a=1;p->b=2;}",
            "struct Slot{int a,b;};void f(struct Slot*volatile p){p->a=1;p->b=2;}",
            "struct Slot{volatile int a;int b;};void f(struct Slot*p){p->a=1;p->b=2;}",
            "typedef volatile int V;typedef V Alias;struct Slot{Alias a;int b;};"
            "void f(struct Slot*p){p->a=1;p->b=2;}",
        ]
        for source in sources:
            with self.subTest(source=source):
                self.assertEqual(moves(source, "statement_order"), [])

    def test_declaration_initializers_are_not_reordered(self):
        self.assertEqual(moves("int f(int a){int x=a++;int y=a++;return x+y;}", "declaration_order"), [])
        self.assertEqual(len(moves("int f(int a){int x;int y;x=a;y=2;return x+y;}", "declaration_order")), 1)

    def test_recompute_stops_at_changed_dependencies(self):
        source = "int f(int a){int x;int y;x=a+1;a=8;y=x;return y;}"
        selected = [m for m in moves(source, "recompute_value") if "recompute x " in m.description]
        self.assertEqual(selected, [])
        source = "int f(int a){int x;int y;x=a+1;y=x;return y;}"
        self.assertTrue(any("recompute x " in m.description for m in moves(source, "recompute_value")))

    def test_copy_stops_at_value_write_or_call(self):
        for source in ("int f(int a){int x;int y;x=a;x=8;y=x;return y;}",
                       "int g(void);int f(int a){int x;int y;x=a;g();y=x;return y;}"):
            selected = [m for m in moves(source, "copy_value") if m.site == "body/block_items[2]"]
            self.assertEqual(selected, [])

    def test_scope_does_not_move_loop_carried_value_into_loop(self):
        source = "int f(int a){int x;while(a>0){if(a==3)x=7;else x=x+1;a--;}return a;}"
        self.assertEqual(moves(source, "scope_narrow"), [])
        source = "int f(int a){int x;if(a){x=7;return x;}return 0;}"
        self.assertEqual(len(moves(source, "scope_narrow")), 1)

    def test_scope_does_not_move_address_taken_local(self):
        source = "int f(int a){int x;int *p;if(a){x=7;p=&x;return *p;}return 0;}"
        self.assertFalse(any("scope of x" in m.description for m in moves(source, "scope_narrow")))

    def test_scope_typedef_binding_is_not_assumed(self):
        source = "typedef int T;int f(int a){if(a){typedef unsigned char T;T x;x=300;return x;}return 0;}"
        self.assertEqual(moves(source, "scope_widen"), [])

    def test_nested_name_is_not_mistaken_for_outer_global(self):
        source = "int x;int f(int a){int y;y=x+1;{int x;x=a;}return y;}"
        self.assertNotIn("x", catalog(source).locals)

    def test_continue_prevents_wrong_for_lowering(self):
        source = "int f(int a){int i;for(i=0;i<5;i++){if(i==a)continue;a++;}return a;}"
        self.assertEqual(moves(source, "loop_form"), [])
        source = "int f(int a){int i;for(i=0;i<5;i++){a++;}return a;}"
        self.assertEqual(len(moves(source, "loop_form")), 1)

    def test_short_circuit_splitting_accepts_side_effects(self):
        source = "int f(int a){if(a++ && a++)return a;return 0;}"
        self.assertEqual(len(moves(source, "split_and")), 1)
        self.assertEqual(moves(source, "commute"), [])

    def test_regions_are_ast_paths(self):
        source = "int f(int a){int x;x=a+1;return a+2;}"
        selected = list(catalog(source).moves(["commute"], region="body/block_items[1]"))
        self.assertEqual(len(selected), 1)
        self.assertEqual(selected[0].site, "body/block_items[1]/rvalue")
        self.assertEqual(list(catalog(source).moves(region=Region(path="nonesuch"))), [])

    def test_source_wrapper_alias_and_helper_are_preserved(self):
        original = '#define public_f private_f\nstatic int helper(void) { return 2; }\n#ifdef NONMATCHING\nint public_f(int a) { return a+1; }\n#else\nINCLUDE_ASM("x", public_f);\n#endif\n'
        preprocessed = 'static int helper(void) { return 2; }\nint private_f(int a) { return a+1; }\n'
        cat = MoveCatalog.from_source(original, "private_f", preprocessed=preprocessed, repo_root=ROOT)
        move = next(cat.moves(["commute"]))
        self.assertTrue(move.source.startswith('#define public_f private_f\nstatic int helper(void) { return 2; }\n#ifdef NONMATCHING\n'))
        self.assertTrue(move.source.endswith('\n#else\nINCLUDE_ASM("x", public_f);\n#endif\n'))
        self.assertIn("int public_f(int a)", move.source)
        self.assertIn("int private_f(int a)", move.preprocessed_source)

    def test_strings_cannot_act_as_comments_or_braces(self):
        source = 'const char *h(void){return "http://x/{y}";}\nint f(int a){return a+1;}\n'
        move = next(catalog(source).moves(["commute"]))
        self.assertTrue(move.source.startswith('const char *h(void){return "http://x/{y}";}\n'))

    def test_no_goto_with_out_of_scope_return_value(self):
        source = "int f(int a){if(a){int x;x=a;return x;}return 0;}"
        self.assertEqual(moves(source, "return_to_goto"), [])

    def test_epilogue_does_not_read_return_local_on_existing_fallthrough(self):
        # f(0), with the call value discarded, never reads uninitialized x.
        source = "int f(int a){int x;if(a){x=5;return x;}}"
        before = catalog(source)
        move = next(before.moves(["return_to_goto"]))
        after = catalog(move.source)

        def return_read_on_zero_path(cat):
            statements = cat.fn.body.block_items
            labels = {node.name: i for i, node in enumerate(statements)
                      if isinstance(node, cat.ca.Label)}
            pc = 0
            for _ in range(len(statements) * 2):
                if pc >= len(statements):
                    return None
                node = statements[pc]
                if isinstance(node, cat.ca.Label):
                    node = node.stmt
                if isinstance(node, cat.ca.Return):
                    return node.expr
                if isinstance(node, cat.ca.Goto):
                    pc = labels[node.name]
                else:
                    # The only If is the original test of a=0, with no else.
                    pc += 1
            self.fail("epilogue unexpectedly introduced a cycle")

        self.assertIsNone(return_read_on_zero_path(before))
        self.assertIsNone(return_read_on_zero_path(after))

    def test_epilogue_does_not_jump_into_vla_scope(self):
        source = "int f(int a){if(a<0)return a;int values[a+1];return 0;}"
        self.assertEqual(moves(source, "return_to_goto"), [])

    def test_pin_removal_rejects_inline_asm_dependency(self):
        try:
            plain = catalog('int f(int a){register int x asm("r2")=a;return x;}')
        except MoveError as exc:
            if "GNU C" in str(exc):
                self.skipTest("permuter GNU parser not installed")
            raise
        self.assertEqual(len(list(plain.moves(["remove_pin"]))), 1)
        asm = catalog('int f(int a){register int x asm("r2")=a;asm("nop");return x;}')
        self.assertEqual(list(asm.moves(["remove_pin"])), [])

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_field_store_permutations_preserve_members_with_independent_compiler(self):
        prefix = ("typedef unsigned char u8; typedef unsigned short u16;"
                  "struct Slot { u8 enter,state;u16 stateFrame;u8 direction,untouched; };\n")
        function = ("void f(struct Slot *slot){slot->state=0;slot->stateFrame=0;"
                    "slot->enter=1;slot->direction=0;}")
        candidates = moves(prefix + function, "statement_order")
        self.assertEqual(len(candidates), 3)
        definitions = [prefix, function.replace("void f(", "void original(", 1)]
        definitions.extend(move.function_source.replace("void f(", f"void candidate_{i}(", 1)
                           for i, move in enumerate(candidates))

        class Slot(ctypes.Structure):
            _fields_ = [("enter", ctypes.c_ubyte), ("state", ctypes.c_ubyte),
                        ("stateFrame", ctypes.c_ushort), ("direction", ctypes.c_ubyte),
                        ("untouched", ctypes.c_ubyte)]

        with tempfile.TemporaryDirectory(prefix="matchsearch-field-stores-") as directory:
            root = Path(directory)
            (root / "cases.c").write_text("\n".join(definitions))
            run = subprocess.run(["cc", "-std=c99", "-shared", "-fPIC", "-O2",
                                  str(root / "cases.c"), "-o", str(root / "cases.so")],
                                 capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stderr)
            library = ctypes.CDLL(str(root / "cases.so"))
            original = library.original
            original.argtypes, original.restype = [ctypes.POINTER(Slot)], None
            for i, move in enumerate(candidates):
                transformed = getattr(library, f"candidate_{i}")
                transformed.argtypes, transformed.restype = [ctypes.POINTER(Slot)], None
                for initial in (0, 1, 127, 255):
                    a = Slot(initial, initial, initial * 257, initial, initial)
                    b = Slot(initial, initial, initial * 257, initial, initial)
                    original(ctypes.byref(a))
                    transformed(ctypes.byref(b))
                    self.assertEqual(tuple(getattr(a, name) for name, _ in Slot._fields_),
                                     tuple(getattr(b, name) for name, _ in Slot._fields_), move.source)
                    self.assertEqual(b.untouched, initial)

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_guarded_candidates_preserve_observed_returns(self):
        sources = [
            "int f(int a,int b){int x;int y;x=a+1;y=b+2;if(x&&y)return x+y;return y;}",
            "int f(int a,int b){unsigned char x;int y;x=a+300;y=x;return x+y+b;}",
            "int f(int a,int b){int i;int x;x=0;for(i=0;i<5;i++){x=x+a;}return x+b;}",
            "int f(int a,int b){int x;if(a){x=b+1;return x;}return b;}",
            "int f(int a,int b){if(a++ && b++)return a+b;return a-b;}",
            "int f(int a,int b){if(a<b)return a;else return b;}",
        ]
        candidates = [(source, move) for source in sources for move in catalog(source).moves()]
        # Compile every candidate as a separate symbol in one shared object.
        definitions = []
        originals = {}
        for i, (source, move) in enumerate(candidates):
            if source not in originals:
                name = f"original_{len(originals)}"
                originals[source] = name
                definitions.append(source.replace("int f(", f"int {name}(", 1))
            definitions.append(move.source.replace("int f(", f"int candidate_{i}(", 1))
        with tempfile.TemporaryDirectory(prefix="matchsearch-moves-") as directory:
            root = Path(directory)
            (root / "cases.c").write_text("\n".join(definitions))
            command = ["cc", "-std=c99", "-shared", "-fPIC", "-O0", str(root / "cases.c"), "-o", str(root / "cases.so")]
            run = subprocess.run(command, capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stderr)
            library = ctypes.CDLL(str(root / "cases.so"))
            for i, (source, move) in enumerate(candidates):
                original = getattr(library, originals[source])
                transformed = getattr(library, f"candidate_{i}")
                original.argtypes = transformed.argtypes = [ctypes.c_int, ctypes.c_int]
                original.restype = transformed.restype = ctypes.c_int
                for a in (-300, -256, -1, 0, 1, 255, 256, 300):
                    for b in (-3, 0, 1, 2):
                        self.assertEqual(original(a, b), transformed(a, b),
                                         (move.family, move.description, a, b, move.source))


if __name__ == "__main__":
    unittest.main()
