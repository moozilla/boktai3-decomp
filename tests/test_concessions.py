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

    def test_volatile_typedef_locals(self):
        for alias in ("vu8", "vu16", "vu32", "vs8", "vs16", "vs32"):
            with self.subTest(alias=alias):
                self.assertEqual(kinds("void f(void) { " + alias + " zero; }"), ["volatile"])
                self.assertEqual(kinds("void f(void) { " + alias + " values[2] = {0}; }"), ["volatile"])
                self.assertEqual(kinds("void f(void) { " + alias + " *reg = REG_ADDR; }"), [])
        self.assertEqual(kinds("void f(vu32 *zero) { *zero = 0; }"), [])
        self.assertEqual(kinds("// vu32 zero;\nvoid f(void) { u32 zero; }"), [])
        for alias in ("vu8", "vu16", "vu32", "vu64", "vs8", "vs16", "vs32", "vs64",
                      "vbool8", "vbool16", "vbool32"):
            for declaration in (f"{alias} zero;", f"{alias} zero = 0;",
                                f"const {alias} zero = 0;", f"{alias} const zero = 0;",
                                f"{alias} buffer[4];", f"{alias} first, second;",
                                f"volatile {alias} zero = 0;"):
                with self.subTest(declaration=declaration):
                    self.assertEqual(kinds(f"void f(void) {{ {declaration} }}"), ["volatile"])

    def test_volatile_typedef_hardware_and_noncode(self):
        source = '''typedef vu32 HardwareWord;
extern vu16 gInterruptFlags;
struct DmaRegisters { vu32 source, destination, control; };
static inline void f(void)
{
    vu32 *dma = (vu32 *)0x040000D4;
    const vu16 *status = (const vu16 *)0x04000004;
    /* vu32 zero; */
    Print("vu32 zero;");
}
'''
        self.assertEqual(kinds(source), [])

    def test_volatile_typedef_macro_and_line_numbers(self):
        source = '#define CLEAR() do { vu32 zero = 0; Copy(&zero); } while (0)\n\nvoid f(void)\n{\n    vu16 value;\n}\n'
        self.assertEqual(c.sites(source, kinds=("volatile",)),
                         [c.Site(1, "volatile"), c.Site(5, "volatile")])
        self.assertEqual(c.sites(source, kinds=("pin",)), [])

    def test_volatile_typedef_allow_and_library_rules(self):
        source = "void f(void) {\n    // concession-ok(volatile): the DMA fill source must be stored in memory\n    vu32 zero = 0;\n}\n"
        self.assertEqual(c.sites(source), [c.Site(3, "volatile", True)])
        self.assertEqual(c.lint(source), [])
        self.assertEqual(kinds("void f(void) { vu32 zero; }", "src/lib/runtime.c"), [])

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


def warns(text, path=None):
    return [s.kind for s in c.warnings(c.sites(text, path))]


class Warnings(unittest.TestCase):
    def test_once_loop_with_a_body_but_not_in_a_macro(self):
        self.assertEqual(warns("void f(void) {\n    do {\n        zero = 0;\n    } while (0);\n}\n"), ["once-loop"])
        self.assertEqual(warns("#define SET(x) do { x = 0; } while (0)\nvoid f(void) {\n    SET(a);\n}\n"), [])
        self.assertEqual(kinds("void f(void) {\n    do { } while (0);\n}\n"), ["empty-loop"])

    def test_small_forms(self):
        self.assertEqual(warns("void f(void) {\n    int zero = 0;\n    a = ({ int v = t; v; });\n    *(&z) = 1;\n    u32 *new_var;\n}\n"),
                         ["const-local", "stmt-expr", "addr-deref", "placeholder-name"])

    def test_const_local_requires_a_declaration_not_a_jump_or_return(self):
        for statement in ("goto zero;", "goto one;", "return zero;", "return one;", "return *zero;"):
            with self.subTest(statement=statement):
                self.assertEqual(warns("int f(void) {\n    " + statement + "\n}\n"), [])
        # Typedef names still count, and returning a real local must not add a second report.
        self.assertEqual(warns("int f(void) {\n    FlagWord zero = 0;\n    return zero;\n}\n"),
                         ["const-local"])
        self.assertEqual(warns("int f(void) {\n    unsigned int one = 1;\n    return one;\n}\n"),
                         ["const-local"])

    def test_const_local_number_names(self):
        for decl in ("u32 zeroX = 0;", "u32 two = 2;", "int four;", "s32 minusOne = -1;", "u32 one_ = 1;"):
            with self.subTest(decl=decl):
                self.assertEqual(warns("void f(void) {\n    " + decl + "\n    g();\n}\n"), ["const-local"])
        for decl in ("u32 zeroed;", "u32 oneshot;", "u32 tenth;"):
            with self.subTest(decl=decl):
                self.assertEqual(warns("void f(void) {\n    " + decl + "\n    g();\n}\n"), [])

    def test_grouped_literal_initializers(self):
        body = """void draw(void) {
    u32 redBlue = 0x7c1f, product = 0xf83e0;
    int i = 0, limit = 8;
    int mutable = 3, value = update(&mutable);
    int nested = consume('(', pair(i, value = 7)), one = 1;
    u32 scales[] = {4, 8}, mask = 31;
    i++;
}
"""
        sites = c.warnings(c.sites(body))
        self.assertEqual([(s.line, s.kind) for s in sites],
                         [(2, "literal-local"), (3, "literal-local"), (5, "const-local"),
                          (6, "literal-local")])

    def test_member_write_does_not_modify_a_same_named_local(self):
        body = """void update(struct Actor *actor, struct Sprite sprite) {
    int state = 4;
    actor -> state = state;
    sprite.state++;
}
"""
        self.assertEqual(warns(body), ["literal-local"])

    def test_number_named_record_members_are_not_locals(self):
        body = """struct Header {
    int one;
};
void clear(void) {
    struct {
        struct { int value; } inner;
        u32 zero;
    } scratch;
    u32 zero = 0;
}
"""
        self.assertEqual(warns(body), ["const-local"])

    def test_literal_local_is_never_changed(self):
        self.assertEqual(warns("void f(u32 a) {\n    u32 mask = 0x17;\n    if (mask & a)\n        g();\n}\n"),
                         ["literal-local"])
        # a counter, an accumulator, something passed by address, and a later scope's own variable
        for body in ("    int i = 0;\n    while (i < 4)\n        g(i++);\n",
                     "    u32 sum = 0;\n    sum += a;\n    g(sum);\n",
                     "    u32 out = 0;\n    h(&out);\n    g(out);\n",
                     "    s32 state = 1;\n    if (a)\n        state = 2;\n    g(state);\n"):
            with self.subTest(body=body):
                self.assertEqual(warns("void f(u32 a) {\n" + body + "}\n"), [])
        # only the block the declaration is in counts
        self.assertEqual(warns("void f(u32 a) {\n    {\n        u32 bit = 8;\n        g(bit);\n    }\n    {\n        u32 bit;\n        bit = a;\n        g(bit);\n    }\n}\n"),
                         ["literal-local"])

    def test_volatile_view_not_hardware(self):
        self.assertEqual(warns("void f(u16 *p) {\n    u32 a = *(volatile u16 *)p;\n}\n"), ["volatile-view"])
        self.assertEqual(warns("void f(void) {\n    u32 a = *(volatile u16 *)REG_ADDR_IE;\n    u32 b = *(volatile u16 *)0x04000200;\n}\n"), [])

    def test_local_union_defined_in_a_function(self):
        self.assertEqual(warns("void f(void) {\n    union {\n        u32 mask;\n        u32 *cursor;\n    } r;\n}\n"), ["local-union"])
        self.assertEqual(warns("void f(void) {\n    union Fixed value;\n}\n"), [])

    def test_warnings_do_not_score_and_library_is_exempt(self):
        text = "void f(void) {\n    int zero = 0;\n}\n"
        self.assertEqual(kinds(text), [])
        self.assertEqual(warns(text, "src/lib/m4a.c"), [])

    def test_a_justification_silences_and_an_admission_does_not(self):
        good = "void f(void) {\n    do {\n        a = 0;\n    } while (0);  // concession-ok(once-loop): expansion of the SDK's DmaClear macro\n}\n"
        bad = good.replace("expansion of the SDK's DmaClear macro", "keeps the table in r4 through reload")
        self.assertEqual(warns(good), [])
        self.assertEqual(c.lint(good), [])
        self.assertTrue(any("admission" in p for p in c.lint(bad)))


class Compiler(unittest.TestCase):
    """dead-code and proved-zero, from scope's instrumented compiler (skipped when it isn't built)."""

    def kinds_of(self, body):
        import deadcode
        if not deadcode.available():
            self.skipTest("scope's compiler is not built")
        os.makedirs(deadcode.CACHE, exist_ok=True)
        path = os.path.join(deadcode.CACHE, f"_test_{os.getpid()}.c")
        with open(path, "w") as f:
            f.write("typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32; typedef int s32;\n"
                    "void g(u32); extern u8 gByte; extern u16 gHalf; extern u32 gWord; extern const u16 gTable[];\n" + body)
        try:
            return sorted({s[0] for s in deadcode.compile_unit(os.path.relpath(path, deadcode.ROOT))})
        finally:
            os.remove(path)

    def test_unreachable_arm_on_a_narrow_value(self):
        self.assertEqual(self.kinds_of("void f(void) {\n    u32 raw = gHalf;\n    if ((s32)raw < 0) {\n        g(raw);\n        return;\n    }\n    g(1);\n}\n"), ["dead-code"])

    def test_respelling_the_guard_does_not_hide_it(self):
        self.assertEqual(self.kinds_of("void f(void) {\n    u32 raw = gByte;\n    if (raw & 0x100) {\n        gWord = raw;\n    }\n    g(1);\n}\n"), ["dead-code", "proved-zero"])

    def test_live_code_and_syntactic_dead_code_are_quiet(self):
        self.assertEqual(self.kinds_of("void f(void) {\n    u32 raw = gHalf;\n    if (raw > 300) {\n        g(raw);\n    }\n    if (0) {\n        g(2);\n    }\n}\n"), [])

    def test_proved_zero(self):
        self.assertEqual(self.kinds_of("void f(u32 i) {\n    const u16 *t = (const u16 *)((u32)gTable + ((u16)i >> 16));\n    g(t[i & 3]);\n}\n"), ["proved-zero"])
        self.assertEqual(self.kinds_of("void f(u32 i) {\n    g((i & 0xFF00) >> 8);\n}\n"), [])

    def test_rejected_narrow_load_combinations_are_not_zero_proofs(self):
        # Serial, EEPROM and RTC bit reads from three real source units.
        # combine tries, then rejects, a narrowing with a zero intermediate;
        # none of these C expressions is identically zero.
        bodies = (
            "void f(void) {\n    if (*(volatile u16 *)0x04000128 & 0x40) g(1);\n}\n",
            "void f(void) {\n    if (*(volatile u16 *)0x0D000000 & 1) g(1);\n}\n",
            "void f(void) {\n    u8 bit = (u16)(*(volatile u16 *)0x080000C4 & 2) >> 1;\n    g(bit);\n}\n",
        )
        for body in bodies:
            with self.subTest(body=body):
                self.assertEqual(self.kinds_of(body), [])

    def test_signed_division_sign_correction_is_not_a_zero_proof(self):
        # (sample >> 3) % 2 on a halfword read as s32: expand_divmod's shifts by 31 are always
        # zero, the remainder is not. A shift the C wrote that is zero still reports.
        head = "extern u16 gHalf;\nstatic inline s32 sample(void) { return gHalf; }\n"
        self.assertEqual(self.kinds_of(head + "void f(void) {\n    g((sample() >> 3) % 2);\n}\n"), [])
        self.assertEqual(self.kinds_of(head + "void f(void) {\n    g((sample() >> 16) % 2);\n}\n"), ["proved-zero"])

    def test_real_zero_proofs_survive_volatile_reads(self):
        for expression in ("(*(volatile u16 *)0x04000128 & 31) >> 5",
                           "*(volatile u16 *)0x04000128 >> 16"):
            with self.subTest(expression=expression):
                self.assertEqual(self.kinds_of("void f(void) {\n    g(" + expression + ");\n}\n"),
                                 ["proved-zero"])


if __name__ == "__main__":
    unittest.main()
