"""Small source fixtures for compiler layouts, call evidence and gated merges."""
from pathlib import Path
import sys
import subprocess
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import keepcheck
import layoutmerge as lm
import struct_evidence as evidence
import structfamilies as families
import structmerge as merge


TYPES = """typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
"""
A = TYPES + """
struct Body { u16 phase; u16 flags; };
struct Host {
    u8 unused[4];
    struct Body body;
    u8 bytes[8];
};
void Tick(struct Body *arg);
void Host_Run(struct Host *p) {
    Tick((struct Body *)&p->bytes[2]);
    Tick((struct Body *)((u8 *)p + (2 << 1)));
    Tick((struct Body *)((u8 *)p + 0));
    Tick(&p->body);
}
"""
B = TYPES + """
struct Flat {
    u8 pad0[4];
    s16 low;
    u16 flags;
    u16 tail[4];
};
void Accept(void *p);
void Step(struct Flat *p) { Accept(p); }
"""


def parse(text, path):
    pre = f'# 1 "{path}"\n' + text
    body, ast, tu, error = evidence.parse(pre, path)
    assert error is None, error
    return pre, body, ast, tu


def measure(text, path):
    _, body, ast, tu = parse(text, path)
    selected = evidence.selected_records(tu, path)
    views = lm.probe_records(body, tu, selected)
    for key, v in views.items():
        _, node = selected[key]
        for d, m in zip(node.decls, v["members"]):
            t = tu.resolve(d.type)
            m["record"] = evidence.record_of(tu, getattr(t, "type", None) if m["array"] else d.type)
            m.update(headers=[], private=[], self_ref=False)
        v.update(file=path, id=key, known=families.inhabited(v))
    return views


class SourceEvidenceTests(unittest.TestCase):
    def test_pointer_arithmetic_and_array_member(self):
        path = "src/moved/a.c"
        _, _, ast, tu = parse(A, path)
        refs = evidence.References(ast, tu, path)
        self.assertEqual([call[5][1:] for call in refs.calls], [(("bytes", 2), 0), ((), 4), ((), 0), (("body",), 0)])
        self.assertEqual([call[6] for call in refs.calls], [f"{path}:struct Body"] * 4)
        self.assertTrue(families.ADDRESS_CANDIDATE.search("f(&p->bytes[0]);"))
        self.assertTrue(families.ADDRESS_CANDIDATE.search("f((Thing *) ((u8 *)p + 0x10));"))
        self.assertFalse(families.ADDRESS_CANDIDATE.search("struct Thing *p = (void *)(base + n);"))

    def test_pointer_member_and_nonconstant_offsets(self):
        text = TYPES + """
struct Item { u16 x; };
struct Owner { struct Item *other; u8 a[8]; };
void Tick(struct Item *);
void F(struct Owner *p, int n) {
    Tick(&p->other[n]);
    Tick((struct Item *)((u8 *)p + n));
    Tick(p->other);
}
"""
        _, _, ast, tu = parse(text, "src/a.c")
        refs = evidence.References(ast, tu, "src/a.c")
        self.assertEqual([r[5] for r in refs.calls], [("src/a.c:struct Item", (), 0)])

    def test_distinct_layouts_need_object_evidence(self):
        a = "src/a.c:struct PlayerWork"
        b = "src/b.c:struct PlayerWork"
        view = {"size": 16, "kind": "struct", "spell": "struct PlayerWork", "known": [
            {"off": 0, "size": 4, "base": "int", "bits": None, "name": "state"}]}
        views = {a: view, b: view}
        from collections import defaultdict
        self.assertEqual(families.family_components({a, b}, views, {}, defaultdict(set)), [])
        self.assertEqual(families.family_components({a, b}, views, {("unit", "Player"): {a, b}},
                                                    defaultdict(set)), [[a, b]])
        views[b] = {**view, "kind": "union"}
        self.assertEqual(families.family_components({a, b}, views, {}, defaultdict(set, {
            (a, b): {("flow", "A parameter 1")}})), [])

    def test_scan_cache_rechecks_changed_source_or_version(self):
        rel = "src/moved/a.c"
        with tempfile.TemporaryDirectory() as tmp, patch.object(families, "ROOT", Path(tmp)), \
                patch.object(families, "scan_file", return_value={"file": rel}) as scan:
            families.scan_cached((rel, "source", b"compiler 1"))
            families.scan_cached((rel, "source", b"compiler 1"))
            self.assertEqual(scan.call_count, 1)
            families.scan_cached((rel, "new source", b"compiler 1"))
            families.scan_cached((rel, "source", b"compiler 2"))
            self.assertEqual(scan.call_count, 3)


@unittest.skipUnless(Path(lm.AGBCC).is_file(), "agbcc required for layout probes")
class CompilerTests(unittest.TestCase):
    def test_probe_compatible_headers_once_and_reuse_the_results(self):
        one = "struct A { u16 flags; };\n"
        two = "struct B { struct A a; u16 next; };\n"
        pre = TYPES + '# 1 "include/a.h"\n' + one + '# 1 "include/b.h"\n' + two
        jobs = [("include/a.h", one, b"version"), ("include/b.h", two, b"version")]
        with tempfile.TemporaryDirectory() as tmp, patch.object(families, "OUT", Path(tmp)), \
                patch.object(lm, "preprocess", return_value=pre) as cpp:
            first = families.scan_header_batch(jobs)
            second = families.scan_header_batch(jobs)
            self.assertEqual(cpp.call_count, 1)
        self.assertEqual(first, second)
        b = first[1]["records"]["include/b.h:struct B"]
        self.assertEqual((b["size"], b["members"][0]["record"]), (8, "include/a.h:struct A"))

    def test_real_offsets_nested_types_and_votes(self):
        path = "src/moved/a.c"
        views = measure(A, path) | measure(B, "src/moved/b.c")
        host, flat = views[f"{path}:struct Host"], views["src/moved/b.c:struct Flat"]
        self.assertEqual(host["size"], 16)
        self.assertEqual(families.address_offset((host["id"], ["bytes", 2], 0), views), (10, None))
        self.assertEqual(families.address_offset((host["id"], ["body"], 0), views), (4, f"{path}:struct Body"))
        self.assertIsNone(families.address_offset((host["id"], ["bytes", 8], 0), views))
        rows = {row["offset"]: row for row in families.layout_rows([host, flat])}
        self.assertEqual(rows[4]["votes"], [("body.phase", "u16", 1), ("low", "s16", 1)])
        self.assertIn("signedness", rows[4]["conflicts"])
        self.assertIn("nested/flat", rows[4]["conflicts"])
        _, _, ast, tu = parse(A, path)
        refs = evidence.References(ast, tu, path)
        with patch.object(families.ramlayout, "load", return_value=[]):
            _, _, _, embeds, _ = families.flow(
                [{"file": path, "calls": refs.calls, "params": refs.params}], views, {})
        self.assertEqual({(off, declared, len(sites)) for (_, off, _, declared), sites in embeds.items()},
                         {(0, False, 1), (4, False, 1), (4, True, 1), (10, False, 1)})

    def test_rom_vote_precedes_majority_and_padding_stays_padding(self):
        text = TYPES + "struct R { u8 pad0[4]; u16 flags; };\n"
        texts = [text, text.replace("u16 flags", "s16 value"), text.replace("u16 flags", "s16 value")]
        views = [next(iter(measure(src, f"src/v{i}.c").values())) for i, src in enumerate(texts)]
        f = {"kind": "struct", "size": views[0]["size"], "loads": [(4, 2, False, "Tick", 0x8000010)]}
        generated, expected, _, _ = merge.plan_header(f, views, "R", Path("include/r.h"))
        self.assertIn("u16 flags", generated)
        self.assertIn("u8 pad0[0x4]", generated)
        self.assertIn("confidence=low", generated)
        self.assertEqual(expected, {"flags": (4, 2)})
        self.assertEqual(merge.probe_header(generated, "struct R", expected, views[0]["size"], views[0]["align"])
                         ["size"], views[0]["size"])

    def test_field_rename_leaves_unrelated_member_on_same_line(self):
        path = "src/moved/a.c"
        old_text = TYPES + """
struct R { u16 old; };
struct Q { u16 old; };
int X(struct R *r, struct Q *q) { return r->old + q->old; }
"""
        new_text = TYPES + "struct S { u16 value; };"
        old = measure(old_text, path)[f"{path}:struct R"]
        new = measure(new_text, "include/s.h")["include/s.h:struct S"]
        with patch.object(lm, "preprocess", return_value=parse(old_text, path)[0]):
            result = merge.rewrite_unit(old_text, old, new, Path("include/s.h"), "struct S")
        self.assertIn("struct S *r", result)
        self.assertIn("return r->value + q->old;", result)
        self.assertIn('#include "s.h"', result)

    def test_initializer_with_changed_shape_is_a_holdout(self):
        path = "src/moved/a.c"
        old_text = TYPES + "struct R { u16 old; };\nstruct R data = { 2 };\n"
        old = measure(old_text, path)[f"{path}:struct R"]
        new = measure(TYPES + "struct S { u16 value; };", "include/s.h")["include/s.h:struct S"]
        with patch.object(lm, "preprocess", return_value=parse(old_text, path)[0]):
            with self.assertRaisesRegex(merge.Holdout, "initializer"):
                merge.rewrite_unit(old_text, old, new, Path("include/s.h"), "struct S")

    def test_union_preserves_deliberate_overlays(self):
        text = TYPES + "union W { unsigned word; u16 halves[2]; };\n"
        views = [next(iter(measure(text, f"src/{i}.c").values())) for i in range(2)]
        f = {"kind": "union", "size": views[0]["size"], "loads": []}
        generated, expected, _, _ = merge.plan_header(f, views, "W", Path("include/w.h"))
        self.assertEqual(expected, {"word": (0, 4), "halves": (0, 4)})
        self.assertEqual(merge.probe_header(generated, "union W", expected, views[0]["size"], views[0]["align"])
                         ["kind"], "union")


class GateTests(unittest.TestCase):
    def test_check_surfaces_the_compiler_failure(self):
        error = "stdin:18: structure has no member named `flags'"
        process = subprocess.CompletedProcess([], 1, "", error + "\nCalledProcessError\n")
        with patch.object(keepcheck.subprocess, "run", return_value=process):
            self.assertEqual(keepcheck.check("src/moved/a.c"), (False, error))

    def test_exact_gate_restores_rejected_units(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "test.c"
            path.write_text("before\n")
            with patch.object(keepcheck, "ROOT", tmp), patch.object(keepcheck, "formatted", return_value=None), \
                    patch.object(keepcheck, "compile_warnings", return_value={}), \
                    patch.object(keepcheck, "check", return_value=(False, "MISMATCH")):
                unit = keepcheck.Unit("test.c")
                self.assertFalse(unit.attempt("after\n"))
                self.assertEqual(path.read_text(), "before\n")
                self.assertEqual(unit.last, "MISMATCH")


if __name__ == "__main__":
    unittest.main()
