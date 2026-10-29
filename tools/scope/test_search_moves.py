"""Safety and session-contract tests for the explicit C move set.

Run with build/venv/bin/python -m unittest discover -s tools/scope -p test_search_moves.py.
The compiler execution check compares defined small inputs across every legal
edge; it catches conversion and control-flow errors that AST round trips cannot.
"""
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

import search_moves as moves


class SearchMovesTests(unittest.TestCase):
    def kinds(self, source):
        return {m.kind for m in moves.enumerate_moves(source, "F")}

    def test_snapshot_identity_serialization_and_stale_detection(self):
        source = 'int F(int a,int b) { return a+b; }'
        first = moves.enumerate_moves(source, "F")
        self.assertEqual([m.id for m in first], [m.id for m in moves.enumerate_moves(source, "F")])
        move = moves.Move.from_dict(json.loads(json.dumps(first[0].to_dict())))
        self.assertEqual(moves.apply_move(source, move), moves.apply_move(source, move.id, "F"))
        with self.assertRaisesRegex(moves.MoveError, "stale"):
            moves.apply_move(source + "\n", move)

    def test_no_assumed_purity(self):
        for expression in ('a+G', 'a+*p', 'a+call()', 'a+v', 'a+(b++)'):
            source = 'int F(int a,int b,int *p) { volatile int v; return %s; }' % expression
            self.assertNotIn("commute", self.kinds(source))
        source = 'int F(int a,int b) { int *p=&a; return a+b; }'
        self.assertNotIn("commute", self.kinds(source))
        source = 'int F(int a,int b) { typedef unsigned int Opaque; Opaque x; return x+b; }'
        self.assertNotIn("commute", self.kinds(source))

    def test_struct_members_and_out_of_scope_names_are_not_locals(self):
        source = 'int F(int a,int b){struct {int G;} obj;return G+a;}'
        self.assertNotIn("commute", self.kinds(source))
        source = 'int F(int a,int b){{int G;G=a;}return G+b;}'
        # The inner assignment can introduce a temporary, but the out-of-scope
        # global name in the return must never be treated as that local.
        self.assertNotIn("commute", self.kinds(source))
        source = 'int F(int a,int b){int (*cb)(int G);return G+a;}'
        self.assertNotIn("commute", self.kinds(source))

    def test_independence_is_proved(self):
        self.assertIn("independent-statements", self.kinds('int F(int a,int b){int x;int y;x=a;y=b;return x+y;}'))
        self.assertNotIn("independent-statements", self.kinds('int F(int a,int b){int x;int y;x=a;y=x;return y;}'))
        self.assertNotIn("independent-statements", self.kinds('int F(int a,int b){int x;int y;x=call();y=b;return y;}'))

    def test_signed_reassociation_does_not_introduce_overflow(self):
        self.assertNotIn("reassociate-unsigned", self.kinds('u32 F(u32 u,s32 a,s32 b){return (u+a)+b;}'))
        self.assertIn("reassociate-unsigned", self.kinds('u32 F(u32 u,u32 a,u32 b){return (u+a)+b;}'))
        self.assertNotIn("unsigned-power-two", self.kinds('s32 F(s32 a,s32 b){return a*8;}'))

    def test_guards_aliases_and_neighbors_are_preserved(self):
        source = '#include "global.h"\n#define Alias sub_12345678\n#ifdef NONMATCHING\nint Alias(int a,int b){return a+b;}\n#else\nINCLUDE_ASM("x",sub_12345678);\n#endif\nint Other(void){return 7;}\n'
        move = moves.enumerate_moves(source, "sub_12345678")[0]
        out = moves.apply_move(source, move)
        self.assertTrue(out.startswith('#include "global.h"\n#define Alias sub_12345678\n#ifdef NONMATCHING\n'))
        self.assertTrue(out.endswith('\n#else\nINCLUDE_ASM("x",sub_12345678);\n#endif\nint Other(void){return 7;}\n'))

    def test_source_sensitive_constructs_decline(self):
        for source in ('int F(void){return __LINE__;}', 'int F(void){\n#ifdef X\nreturn 1;\n#endif\n}', 'int F(void){asm("mov r0,r0");return 0;}'):
            with self.assertRaises(moves.MoveError):
                moves.enumerate_moves(source, "F")

    def test_plain_char_and_transitive_textual_macros_decline(self):
        self.assertNotIn("commute", self.kinds('int F(char a,int b){return a+b;}'))
        source = '#define TEXT(x) #x\n#define WRAP(x) TEXT(x)\nint F(int a,int b){WRAP(a+b);return a+b;}'
        with self.assertRaisesRegex(moves.MoveError, "textual"):
            moves.enumerate_moves(source, "F")
        source = '#define LOCATION __LINE__\n#define GET_LOCATION LOCATION\nint F(int a,int b){return GET_LOCATION+a+b;}'
        with self.assertRaisesRegex(moves.MoveError, "textual"):
            moves.enumerate_moves(source, "F")

    def test_macro_declared_local_does_not_acquire_a_false_scalar_type(self):
        source = '#define x *p\nint F(int a,int b){int x;return x+a;}'
        with self.assertRaisesRegex(moves.MoveError, "identifier is a macro"):
            moves.enumerate_moves(source, "F")

    def test_existing_pins_and_empty_barriers_are_retained(self):
        source = 'int F(int a,int b){register int x asm("r0")=a;asm("" : "+r"(x));if(a){return x;}else{return b;}}'
        ms = moves.enumerate_moves(source, "F")
        self.assertIn("invert-if", {m.kind for m in ms})
        for m in ms:
            out = moves.apply_move(source, m)
            self.assertIn('asm("r0")', out)
            self.assertIn('asm("" : "+r"(x))', out)

    @unittest.skipUnless(shutil.which("cc"), "native C compiler unavailable")
    def test_every_edge_preserves_defined_execution(self):
        source = '''u32 F(u32 a,u32 b) {
 u32 x; u32 y; u16 t;
 x = (a + b) + 3U;
 y = b * 8U;
 t = a + b;
 x = t;
 if ((a < b) && (b != 0U)) { x += y; } else { x = x + 1U; }
 while (y > 32U) { y = y / 2U; }
 return (a != 0U) ? (x ^ y) : (y % 8U);
}'''
        prelude = 'typedef unsigned int u32; typedef unsigned short u16;\n'
        driver = '\n#include <stdio.h>\nint main(void){u32 a,b;for(a=0;a<12;a++)for(b=0;b<12;b++)printf("%u\\n",F(a,b));return 0;}\n'
        root = moves.ROOT / "build" / "search-moves"
        root.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=root) as work:
            outputs = []
            candidates = [source] + [moves.apply_move(source, m) for m in moves.enumerate_moves(source, "F")]
            for i, candidate in enumerate(candidates):
                path, binary = Path(work) / ('v%d.c' % i), Path(work) / ('v%d' % i)
                path.write_text(prelude + candidate + driver)
                built = subprocess.run(['cc', '-std=c89', '-O2', str(path), '-o', str(binary)], capture_output=True, text=True)
                self.assertEqual(built.returncode, 0, built.stderr)
                outputs.append(subprocess.check_output([str(binary)]))
            self.assertGreater(len(outputs), 12)
            for output in outputs[1:]:
                self.assertEqual(output, outputs[0])


if __name__ == "__main__":
    unittest.main()
