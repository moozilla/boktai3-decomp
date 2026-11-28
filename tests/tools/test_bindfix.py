"""Planning coverage for tools/bindfix.py (no compiler runs)."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import bindfix as bf

REL = "src/player/bindfix_fixture.c"


class BindfixTests(unittest.TestCase):
    def collect(self, text):
        return bf.collect(REL, text)

    def test_parked_local_test_becomes_typed_accessor_call(self):
        text = """struct Half { u32 pad; u32 actionFlags; };
u32 f(struct Half *actor) {
    u32 mask = 0x100;
    if (actor->actionFlags & mask)
        return 1;
    return 0;
}
"""
        site, = self.collect(text)
        use, = site.uses
        self.assertEqual((use.pointer, use.owner, use.chain, use.op, use.literal),
                         ("actor", "struct Half", "actionFlags", "Test", "0x100"))
        self.assertTrue(site.cuts)
        run = bf.Run(type("A", (), {"dry": True})())
        (edits, new, _), _ = run.plan_edits(REL, text, [site])
        out = bf.folds.edit_text(text, edits[REL])
        self.assertIn("if (Half_TestActionFlags(actor, 0x100))", out)
        self.assertNotIn("u32 mask = ", out)
        self.assertIn("static inline u32 Half_TestActionFlags(struct Half *p, u32 mask)\n{\n"
                      "    return p->actionFlags & mask;\n}", out)
        self.assertLess(out.index("struct Half {"), out.index("Half_TestActionFlags(struct"))

    def test_generic_pointer_helper_on_nested_member(self):
        text = """struct Base { u32 behaviorFlags; };
struct Chase { struct Base base; };
static inline void WordFlags_Clear(u32 *flags, u32 bits)
{
    *flags &= ~bits;
}
void f(struct Chase *x) {
    WordFlags_Clear(&x->base.behaviorFlags, 4);
}
"""
        site, = self.collect(text)
        use, = site.uses
        self.assertEqual((use.pointer, use.chain, use.op), ("x", "base.behaviorFlags", "Clear"))
        self.assertEqual(bf.accessor_name(use.owner, use.chain, use.op), "Chase_ClearBehaviorFlags")

    def test_existing_typed_accessor_is_left_alone(self):
        text = """struct Half { u32 actionFlags; };
static inline void Half_Mark(struct Half *p, u32 mask)
{
    p->actionFlags |= mask;
}
void f(struct Half *p) {
    Half_Mark(p, 8);
}
"""
        site, = self.collect(text)
        self.assertEqual(site.verdict, "already-accessor")

    def test_non_flag_local_is_out_of_scope(self):
        text = """void g(int);
void f(void) {
    int zero = 0;
    g(zero);
}
"""
        sites = self.collect(text)
        self.assertTrue(sites)
        self.assertTrue(all(s.verdict == "out-of-scope" for s in sites))

    def test_comparison_regrouping_is_refused(self):
        self.assertIsNone(bf.expression_use("a == p->flags & mask;", len("a == p->flags & "), "mask"))
        self.assertIsNotNone(bf.expression_use("if (p->flags & mask) x();", len("if (p->flags & "), "mask"))

    def test_bare_block_left_by_the_declaration_is_flattened(self):
        text = "void f(void)\n{\n    x();\n    {\n\n        A_Set(p, 4);\n    }\n    if (y)\n    {\n" \
               "        A_Set(p, 4);\n    }\n}\n"
        self.assertEqual(bf.flatten_blocks(text, {"A_Set"}),
                         "void f(void)\n{\n    x();\n    A_Set(p, 4);\n    if (y)\n    {\n"
                         "        A_Set(p, 4);\n    }\n}\n")

    def test_keep_mask_becomes_clear_with_keep_fallback(self):
        text = """struct Half { u32 flags; };
static inline void Keep(u32 *flags, u32 mask)
{
    *flags &= mask;
}
void f(struct Half *h) {
    Keep(&h->flags, ~0x100);
}
"""
        site, = self.collect(text)
        use, = site.uses
        self.assertEqual((use.op, use.literal, use.keep_literal), ("Clear", "0x100", "~0x100"))
        run = bf.Run(type("A", (), {"dry": True})())
        (edits, _, _), _ = run.plan_edits(REL, text, [site], keep=True)
        self.assertIn("Half_KeepFlags(h, ~0x100);", bf.folds.edit_text(text, edits[REL]))

    def test_redundant_grouping_parentheses_are_dropped(self):
        code = "if ((p->f & m) || q)"
        start, end = code.index("p->f"), code.index(") ||")
        self.assertEqual(bf.unparen_span(code, start, end), (start - 1, end + 1))
        code = "if (p->f & m)"
        self.assertEqual(bf.unparen_span(code, 4, len(code) - 1), (4, len(code) - 1))


if __name__ == "__main__":
    unittest.main()
