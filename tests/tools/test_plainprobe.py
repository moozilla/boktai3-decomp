"""Regression coverage for independent, linter-indexed literal probes."""
import csv
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import plainprobe as pp


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


if __name__ == "__main__":
    unittest.main()
