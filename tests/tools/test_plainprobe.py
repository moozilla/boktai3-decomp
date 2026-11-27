"""Regression coverage for independent, linter-indexed literal probes."""
import csv
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import plainprobe as pp
import plainprobe_rom as rom


class PlainProbeTests(unittest.TestCase):
    def plan(self, text):
        return pp.plan(ROOT / "src/player/plainprobe_fixture.c", text, {})

    def test_separate_calls_on_one_line(self):
        source = """#include "engine/word_flags.h"
u32 f(struct Actor *p) {
    return WordFlags_Test(&p->flags, 4) | WordFlags_Test(&p->more, 4);
}
"""
        a, b = self.plan(source)
        self.assertEqual(a.line, b.line)
        self.assertNotEqual(a.id, b.id)
        self.assertEqual((a.owner, a.field, b.field), ("struct Actor", "flags", "more"))
        self.assertIn(" 4", a.other_uses)
        variant = pp.folds.edit_text(source, a.edits)
        self.assertIn("(p->flags & 4)", variant)
        self.assertIn("WordFlags_Test(&p->more, 4)", variant)

    def test_every_read_is_one_binding_with_two_charged_uses(self):
        source = """u32 f(struct Actor *p) {
    u32 mask;
    mask = 04;
    return (p->flags & mask) | (p->more & mask);
}
"""
        p, = self.plan(source)
        self.assertEqual((p.line, p.spelling, p.value, p.weight), (3, "mask", 4, 2))
        self.assertIn("same binding", p.other_uses)
        self.assertEqual(pp.folds.edit_text(source, p.edits), """u32 f(struct Actor *p) {
    return (p->flags & 04) | (p->more & 04);
}
""")

    def test_grouped_binding_gets_one_row_per_name(self):
        rows = self.plan("""u32 f(u32 x) {
    u32 mask = 8, keep = 16;
    return (x & mask) | (x & keep);
}
""")
        self.assertEqual([r.spelling for r in rows], ["mask", "keep"])
        self.assertTrue(all(r.reason for r in rows))

    def test_boolean_and_compound_operations(self):
        for body, expected in (
            ("if (*p & m) return 1; return 0;", ("*p", "nonzero")),
            ("return (*p & m) != 0;", ("*p", "nonzero")),
            ("p->flags |= m;", ("p->flags", "set")),
            ("p->flags &= ~m;", ("p->flags", "clear")),
            ("*p = m & *p;", ("*p", "and-store")),
        ):
            with self.subTest(body=body):
                self.assertEqual(pp.operation(pp.compact(body), "m"), expected)
        self.assertIsNone(pp.operation("if(p)*p|=m;", "m"))
        self.assertIsNone(pp.operation("return(gFirst|gSecond)&m;", "m"))

    def test_compile_failure_is_skip_and_difference_uses_sharing(self):
        source = """u32 f(u32 x) {
    u32 mask = 8;
    return x & mask;
}
"""
        p, = self.plan(source)

        class Check:
            verdict = "DIFF"

            def compare(self, path, original, variant):
                self.original, self.variant = original, variant
                return self.verdict, "fixture"

        checker = Check()
        sources = {p.path: source}
        self.assertEqual(pp.probe(p, sources, checker), ("BOUND-SINGLE", "fixture"))
        self.assertIn("x & 8", checker.variant)
        p.other_uses = "11:14 8"
        self.assertEqual(pp.probe(p, sources, checker), ("BOUND-SHARED", "fixture"))
        checker.verdict = "SKIP"
        self.assertEqual(pp.probe(p, sources, checker), ("SKIP", "fixture"))

    def test_mask_operand_orientation_and_temporary_clear(self):
        rows = self.plan("""static inline u32 Test(u32 v, u32 m) {
    return v & m;
}
static inline void Clear(u32 *p, u32 m) {
    u32 keep = ~m;
    *p &= keep;
}
u32 f(struct Actor *p) {
    Clear(&p->flags, 8);
    return Test(p->flags, 16) | Test(32, p->flags);
}
""")
        clear, right, left = rows
        self.assertEqual([(p.operation, p.field) for p in rows],
                         [("clear", "flags"), ("and", "flags"), ("and", "flags")])
        self.assertEqual([p.value for p in rows], [8, 16, 32])
        self.assertIn("(p->flags &= ~8)", clear.edits[0][2])
        self.assertTrue(right.edits and left.edits)

    def test_local_helper_argument_and_typed_pointer_alias(self):
        p, = self.plan("""#include "engine/word_flags.h"
void f(struct Actor *p) {
    u32 mask = 8;
    u32 *ptr = &p->flags;
    WordFlags_Set(mask, ptr);
}
""")
        self.assertEqual(p.field_keys, (("struct Actor", "flags", "set"),))

    def test_narrow_unsigned_local_does_not_lose_its_conversion(self):
        p, = self.plan("""u32 f(u32 word) {
    u8 keep = ~3;
    return word & keep;
}
""")
        self.assertFalse(p.edits)

    def test_equal_constant_expression_is_another_use(self):
        p, = self.plan("""u32 f(u32 flags) {
    u32 mask = 16;
    g(1 << 4);
    return flags & mask;
}
""")
        self.assertIn("1 << 4", p.other_uses)

    def test_clear_links_to_inverted_and_store(self):
        p, = self.plan("""static inline void Clear(u32 *p, u32 m) {
    *p &= ~m;
}
void f(struct Actor *p) {
    Clear(&p->flags, 8);
}
""")
        pp.link_rom([p], {}, {("f", "Actor.flags", "and-store", 0xFFFFFFF7): {("08000000", "mask")}})
        self.assertEqual(p.rom_matches, (("08000000", "mask"),))

    def test_baseline_extent_counts_deleted_trailing_bytes(self):
        checker = pp.Checker.__new__(pp.Checker)
        checker.baseline = lambda path, text: (0x08000000, ["f"], b"abcd")
        checker.compile = lambda path, text: (0x08000000, ["f"], b"ab")
        verdict, detail = checker.compare(Path("fixture.c"), "base", "edit")
        self.assertEqual(verdict, "DIFF")
        self.assertIn("2 byte positions", detail)

    def test_embedded_type_aliases_are_only_from_by_value_members(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "record.h"
            path.write_text("""struct Actor { u32 flags; };
struct Child { struct Actor base; struct Actor *ref; };
struct Wrapper { struct Child nested; };
""")
            _, members, ambiguous = rom.records([path])
        self.assertFalse(ambiguous)
        self.assertEqual(rom.canonical("struct Wrapper", "nested.base.flags", members), ("struct Actor", "flags"))
        self.assertEqual(rom.canonical("struct Child", "ref.flags", members), ("struct Child", "ref.flags"))

    def test_sample_and_checkpoint(self):
        sites = [pp.Probe("src/scene/here.c", i, 1, i + 1, "literal-param", 1, 0, "src/scene")
                 for i in range(43)]
        ordered = pp.sample_order(sites)
        self.assertTrue(all(p.sample for p in ordered[:40]))
        self.assertFalse(any(p.sample for p in ordered[40:]))
        p = ordered[0]
        with tempfile.TemporaryDirectory() as tmp:
            output = Path(tmp) / "sites.csv"
            pp.write_csv(output, ordered, {p.id: ("PLAIN", "MATCH")})
            with output.open() as stream:
                rows = list(csv.DictReader(stream))
            self.assertEqual((rows[0]["id"], rows[0]["class"]), (p.id, "PLAIN"))
            self.assertEqual(rows[-1]["class"], "PENDING")

    def test_resume_reuses_plan_and_rebuilds_after_input_change(self):
        sources = {"src/player/fixture.c": """u32 f(struct Actor *p) {
    u32 mask = 8;
    return p->flags & mask;
}
"""}
        with tempfile.TemporaryDirectory() as tmp:
            cache = Path(tmp) / "plan.json"
            original, = pp.cached_sites(cache, "snapshot-one", sources)
            with patch.object(pp, "plan_sites", wraps=pp.plan_sites) as planner:
                restored, = pp.cached_sites(cache, "snapshot-one", sources)
                planner.assert_not_called()
                self.assertEqual(restored, original)
                self.assertEqual(pp.probe_version(restored), pp.probe_version(original))

                updated = {name: text.replace("mask = 8", "mask = 16") for name, text in sources.items()}
                changed, = pp.cached_sites(cache, "snapshot-two", updated)
                planner.assert_called_once_with(updated)
                self.assertEqual(changed.value, 16)

                cache.write_text("{")
                recovered, = pp.cached_sites(cache, "snapshot-two", updated)
                self.assertEqual(recovered, changed)


if __name__ == "__main__":
    unittest.main()
