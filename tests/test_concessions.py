"""tools/concessions.py: the concession kinds, the concession-ok escape hatch and the score."""
import os
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import concessions as c  # noqa: E402


def kinds(text, path=None, **kw):
    return [s.kind for s in c.counted(c.sites(text, path, **kw))]


class Kinds(unittest.TestCase):
    def test_macro_parameter_can_supply_a_hard_register(self):
        import depin
        source = '#define READ(out, reg) register unsigned tmp __asm__(reg) = 4; out = tmp'
        self.assertEqual(kinds(source), ["pin"])
        hint, = depin.hints(source)
        self.assertEqual(depin.drop(source, hint),
                         '#define READ(out, reg) unsigned tmp = 4; out = tmp')
        self.assertEqual(kinds('register unsigned normal; asm(SAVE_CONTEXT);'), [])

    def test_asm_forms(self):
        self.assertEqual(kinds('void f(void) { register int x asm("r4"); }'), ["pin"])
        self.assertEqual(kinds('void f(void) { register int x asm("ip") = 0; }'), ["pin"])
        self.assertEqual(kinds('void f(int x) { asm("" : "+r"(x)); }'), ["operand"])
        self.assertEqual(kinds('void f(void) { asm("" ::: "r4"); }'), ["clobber"])
        self.assertEqual(kinds('void f(void) { asm(""); asm volatile("" ::: "memory"); }'), ["barrier", "barrier"])

    def test_empty_bodies_and_debug_nop(self):
        self.assertEqual(kinds("void f(void) { do {\n} while (0); }"), ["empty-loop"])
        self.assertEqual(kinds("void f(int x) { if (g(x)) { } }"), ["empty-if"])
        self.assertEqual(kinds("void f(int x) { if (g(x)) {\n DEBUG_PRINT();\n } }"), [])
        self.assertEqual(kinds("void f(void) { DEBUG_NOP(); }"), ["debug-nop"])
        self.assertEqual(kinds("void f(void) { do { g(); } while (0); }"), [])

    def test_volatile_objects_not_hardware(self):
        self.assertEqual(kinds("void f(void) { volatile u32 zero = 0; }"), ["volatile"])
        self.assertEqual(kinds("void f(void) { u16 *volatile p = q; }"), ["volatile"])
        self.assertEqual(kinds("void f(s32 *p) { g(*(volatile s32 *)&p->x); }"), ["volatile"])
        self.assertEqual(kinds("void f(void) { volatile u16 *reg = (vu16 *)0x4000000; *(vu16 *)&REG_IE = 1; }"), [])

    def test_comments_are_not_code(self):
        self.assertEqual(kinds('void f(void) { /* asm("") */ g(); } // DEBUG_NOP()'), [])

    def test_tracked_kinds_are_not_counted(self):
        text = "void f(u8 *p) { again: g(); goto again; }"
        self.assertEqual(kinds(text), [])
        self.assertEqual([s.kind for s in c.sites(text)], ["goto"])

    def test_constant_raw_offsets_count_and_variable_ones_do_not(self):
        self.assertEqual(kinds("void f(struct A *p) { *(u16 *)((u8 *)p + 0x172) = 1; }"), ["raw-offset"])
        self.assertEqual(kinds("u16 f(const u16 *t, u32 o) { return *(const u16 *)((const u8 *)t + o); }"), [])

    def test_single_use_macro_site(self):
        text = "#define ONCE(x) do { g(x); } while (0)\nvoid f(int x) { ONCE(x); }"
        self.assertEqual(kinds(text, single_use={"ONCE"}), ["single-use-macro"])

    def test_alias_and_cflags(self):
        self.assertEqual(c.aliases('extern int CpuSetResult(const void *, void *, u32) asm("CpuSet");\n'),
                         {"CpuSetResult": False})
        self.assertIsNone(c.cflags("// CFLAGS: -O2 -mthumb-interwork\n"))
        self.assertFalse(c.cflags("// CFLAGS: -O2 -mthumb-interwork -fno-expensive-optimizations\n").excused)
        self.assertTrue(c.cflags("// CFLAGS: -O0\n", "src/lib/x.c").excused)


class Allow(unittest.TestCase):
    def test_same_line_and_line_above(self):
        same = "void f(void) { DEBUG_NOP();  // concession-ok(debug-nop): the release build kept this hook\n}"
        above = "void f(void) {\n    // concession-ok(debug-nop): the release build kept this hook\n    DEBUG_NOP();\n}"
        self.assertEqual(kinds(same), [])
        self.assertEqual(kinds(above), [])
        self.assertEqual(c.lint(same), [])
        self.assertEqual(c.lint(above), [])

    def test_an_allow_does_not_reach_further(self):
        text = "// concession-ok(debug-nop): the release build kept this hook\n\nvoid f(void) { DEBUG_NOP(); }"
        self.assertEqual(kinds(text), ["debug-nop"])
        self.assertTrue(any("nothing" in p or "no debug-nop" in p for p in c.lint(text)))

    def test_pins_cannot_be_excused(self):
        text = 'void f(void) { register int x asm("r4"); // concession-ok(pin): the original did this\n}'
        self.assertEqual(kinds(text), ["pin"])
        self.assertTrue(any("never" in p for p in c.lint(text)))

    def test_reason_and_kind_are_required(self):
        self.assertTrue(any("reason" in p for p in c.lint("void f(void) { DEBUG_NOP(); // concession-ok(debug-nop)\n}")))
        self.assertTrue(any("unknown" in p for p in c.lint("x; // concession-ok(hack): because it matched better\n")))

    def test_library_sources_are_exempt_from_judgment_calls(self):
        self.assertEqual(kinds("void f(void) { volatile u8 chIdx; }", "src/lib/m4a.c"), [])
        self.assertEqual(kinds('void f(void) { register int x asm("r4"); }', "src/lib/m4a.c"), ["pin"])


class Score(unittest.TestCase):
    def test_share_of_lines_without_a_concession(self):
        body = "void f(void)\n{\n    a();\n    b();\n    c();\n    DEBUG_NOP();\n}\n"
        self.assertEqual(c.score(body, 0), 100.0)
        self.assertEqual(c.score(body, 1), 80.0)  # 5 code lines (the signature counts), 1 with a concession
        self.assertLess(c.score("void f(void) {\n" + "a();\n" * 5000 + "}\n", 1), 100)


class Tree(unittest.TestCase):
    def test_every_allow_comment_is_valid(self):
        """The repository's concession-ok comments name an excusable kind, give a reason and excuse something."""
        sys.path.insert(0, str(ROOT / "tools"))
        from statement_macros import single_use_names
        once = frozenset(single_use_names(ROOT))
        problems = []
        for sub in ("src", "include"):
            for dirpath, _, files in os.walk(ROOT / sub):
                for f in files:
                    if f.endswith((".c", ".h")):
                        path = Path(dirpath, f)
                        text = path.read_text(errors="replace")
                        if "concession-ok" in text:
                            problems += c.lint(text, path.relative_to(ROOT), single_use=once)
        self.assertEqual(problems, [])


if __name__ == "__main__":
    unittest.main()
