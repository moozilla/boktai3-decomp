"""Small source fixtures for compiler layouts, call evidence and gated merges."""
from pathlib import Path
from collections import defaultdict
import sys
import subprocess
import tempfile
import unittest
from types import SimpleNamespace
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import keepcheck
import deadcode
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
        spell, node = selected[key]
        for d, m in zip(node.decls, v["members"]):
            t = tu.resolve(d.type)
            m["record"] = evidence.record_of(tu, getattr(t, "type", None) if m.get("array") else d.type)
            headers, private, forwards, self_ref = evidence.member_dependencies(tu, d, spell)
            private = [name for name in private if name not in {"u8", "u16", "s16", "u32"}]
            m.update(headers=headers, private=private, forwards=forwards, self_ref=self_ref)
        v.update(file=path, id=key, known=families.inhabited(v))
    return views


class SourceEvidenceTests(unittest.TestCase):
    def test_return_evidence_uses_calls_and_resolves_static_names_locally(self):
        definition = "src/allocator.c"
        caller = "src/npc.c"
        header = "include/shared.h"
        base = f"{header}:struct Base"
        view = f"{caller}:struct View"
        decls = f'# 1 "{header}"\nstruct Base {{ unsigned flags; }};\n'
        body = f'# 1 "{definition}"\nstruct Base *Allocate(void) {{ return 0; }}\n'
        defs = evidence.References(*parse(decls + body, definition)[2:], definition)
        script = """
struct View { unsigned flags; };
struct View *Allocate(void);
static struct View *Other(void) { return 0; }
struct View *NeverCalled(void);
void Run(void) { Allocate(); Other(); }
void Shadow(struct View *(*Allocate)(void)) { Allocate(); }
"""
        calls = evidence.References(*parse(script, caller)[2:], caller)
        self.assertEqual(calls.return_calls, [("Run", 6, "Allocate", False, view),
                                               ("Run", 6, "Other", True, view)])
        scans = [{"file": definition, "return_definitions": defs.return_definitions + [("Other", True, base)]},
                 {"file": caller, "return_calls": calls.return_calls, "return_definitions": calls.return_definitions}]
        self.assertEqual(families.returned_views(scans, {base: {}, view: {}}),
                         [{"view": view, "result": base, "function": "Allocate",
                           "sites": [("src/npc.c:6", "Run")]}])

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

    @unittest.skipUnless(hasattr(evidence.c_parser.CParser, "_parse_primary_expression"),
                         "GNU statement expressions need recursive-descent pycparser")
    def test_statement_expression_preserves_embedded_call_evidence(self):
        path = "src/a.c"
        text = TYPES + """
struct Body { u16 flags; };
struct Host { struct Body body; };
void Tick(struct Body *);
unsigned Host_Run(struct Host *p) {
    unsigned direction = ({ Tick(&p->body); 3; }) << 6;
    return direction | ({ Tick(&p->body); 1; });
}
"""
        _, _, ast, tu = parse(text, path)
        refs = evidence.References(ast, tu, path)
        self.assertEqual([call[5] for call in refs.calls], [(f"{path}:struct Host", ("body",), 0)] * 2)
        self.assertEqual([call[6] for call in refs.calls], [f"{path}:struct Body"] * 2)

    def test_distinct_layouts_need_object_evidence(self):
        a = "src/a.c:struct PlayerWork"
        b = "src/b.c:struct PlayerWork"
        view = {"size": 16, "kind": "struct", "spell": "struct PlayerWork", "known": [
            {"off": 0, "size": 4, "base": "int", "bits": None, "name": "state"}]}
        views = {a: view, b: view}
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

    def test_a_shared_type_conflict_is_not_an_extension(self):
        halfword = (2, None, "int", None, None)
        shared = {(0, halfword, False)}
        self.assertTrue(families.extension_of(shared | {(2, halfword, True)}, shared))
        self.assertFalse(families.extension_of(shared | {(0, halfword, True)}, shared))
        self.assertFalse(families.extension_of(shared | {(1, halfword, False)}, shared))

    def test_shared_probe_scope_defaults_to_all_records_and_accepts_named_headers(self):
        headers = {
            "include/a.h": "struct A { int value; };",
            "include/b.h": "union B { int value; };",
            "include/alias.h": "typedef int Alias;",
            "src/internal.h": "struct Local { int value; };",
        }
        self.assertEqual([path for path, _ in families.shared_scan_inputs(headers, None)],
                         ["include/a.h", "include/b.h"])
        requested = [Path("include/alias.h"), ROOT / "include/alias.h", Path("include/b.h")]
        self.assertEqual(families.shared_scan_inputs(headers, requested),
                         [(path, headers[path]) for path in ("include/alias.h", "include/b.h")])
        with self.assertRaisesRegex(ValueError, "below include/"):
            families.shared_scan_inputs(headers, [Path("src/internal.h")])
        with self.assertRaisesRegex(ValueError, "shared header not found"):
            families.shared_scan_inputs(headers, [Path("include/missing.h")])

    def test_report_groups_load_widths_and_signedness_at_each_offset(self):
        family = {"id": "Test", "files": 0, "size": 8, "records": [], "warnings": 0,
                  "score": 0, "shared": [], "evidence": [],
                  "rows": [{"offset": n, "votes": [], "conflicts": []} for n in (0, 2, 4)],
                  "loads": [(4, 2, True, "F", 0x8000000), (0, 4, None, "F", 0x8000002),
                            (4, 1, False, "F", 0x8000004), (0, 4, None, "F", 0x8000006),
                            (0, 4, False, "G", 0x8000008)]}
        table = [line for line in families.family_lines(family) if line.startswith("| `0x")]
        self.assertEqual(table, ["| `0x0` |  |  | word32 × 2, u32 × 1 |",
                                 "| `0x2` |  |  |  |",
                                 "| `0x4` |  |  | u8 × 1, s16 × 1 |"])

    def test_warning_counts_honor_literal_param_exceptions(self):
        helper = """static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
void Test(u32 *flags)
{
    TestFlags(flags, 1);
}
"""
        allow = "// concession-ok(literal-param): the SDK header documents this accessor by name\n"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "src").mkdir()
            (root / "src/a.c").write_text(allow + helper)
            (root / "src/b.c").write_text(helper)
            with patch.object(families, "ROOT", root):
                self.assertEqual(families.warning_counts(["src/a.c", "src/b.c"], False, 1),
                                 {"src/a.c": 0, "src/b.c": 1})

    def test_warning_counts_skip_the_call_index_without_eligible_definitions(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "src").mkdir()
            (root / "src/a.c").write_text("void Test(void) {}\n")
            with patch.object(families, "ROOT", root), \
                    patch.object(families.concessions, "literal_params") as params:
                self.assertEqual(families.warning_counts(["src/a.c"], False, 1), {"src/a.c": 0})
                params.assert_not_called()


@unittest.skipUnless(Path(lm.AGBCC).is_file(), "agbcc required for layout probes")
class CompilerTests(unittest.TestCase):
    def test_shared_header_rejects_same_size_layout_drift_from_an_include(self):
        path = "include/shared.h"
        key = f"{path}:struct Shared"
        source = '#include "entry.h"\nstruct Shared { struct Entry entry; };\n'
        with tempfile.TemporaryDirectory(dir=ROOT / "build") as tmp:
            root = Path(tmp)
            (root / "include").mkdir()
            (root / "build").mkdir()
            (root / "include/global.h").write_text(TYPES)
            (root / path).write_text(source)
            entry = root / "include/entry.h"
            entry.write_text("struct Entry { u16 phase; u16 target; };\n")
            with patch.object(families, "ROOT", root), patch.object(lm, "ROOT", root):
                scan = families.scan_file(path, source)
                catalog = {"files": {path: scan["digest"]}, "records": scan["records"]}
                family = {"shared": [(key, "equals", "fixture", 2)]}
                args = SimpleNamespace(use_shared="struct Shared")
                view = merge.header_choice(catalog, family, [], args)[3]
                self.assertEqual([(leaf["name"], leaf["off"]) for leaf in view["known"]],
                                 [("entry.phase", 0), ("entry.target", 2)])
                entry.write_text("struct Entry { u16 target; u16 phase; };\n")
                changed = families.scan_file(path, source)["records"][key]
                self.assertEqual(changed["size"], view["size"])
                with self.assertRaisesRegex(merge.Holdout, "shared layout changed"):
                    merge.header_choice(catalog, family, [], args)

    def test_layout_rows_report_direct_shapes_before_the_first_named_leaf(self):
        entry = TYPES + "struct Entry { u8 pad0[4]; u16 phase; };\n"
        array = entry + "struct Host { unsigned state; struct Entry linked[2]; };\n"
        union = entry + "union Slots { struct Entry items[2]; };\n" \
            "struct Host { unsigned state; union Slots orbs; };\n"
        views = [measure(src, path)[f"{path}:struct Host"]
                 for src, path in ((array, "src/a.c"), (array, "src/b.c"), (union, "src/c.c"))]

        rows = {row["offset"]: row for row in families.layout_rows(views)}
        self.assertEqual(rows[4]["votes"], [])
        self.assertIn("aggregate layout", rows[4]["conflicts"])
        self.assertEqual(rows[4]["member_votes"], [("linked", "struct Entry [2]", 2),
                                                  ("orbs", "union Slots", 1)])

    def test_layout_rows_accept_union_alternatives_shared_by_every_copy(self):
        entry = TYPES + "struct Entry { u16 phase; }; typedef struct Entry EntryAlias; "
        union = "union Slot { %s item; unsigned word; };"
        views = [measure(entry + union % spell, path)[f"{path}:union Slot"]
                 for spell, path in (("struct Entry", "src/a.c"), ("EntryAlias", "src/b.c"))]
        row = families.layout_rows(views)[0]
        self.assertNotIn("aggregate layout", row["conflicts"])
        self.assertNotIn("member_votes", row)

    def test_embedded_members_inherit_evidence_only_at_the_same_slot(self):
        def host(path, child_fields, member):
            return measure(TYPES + f"""
struct Link {{ {child_fields} }};
struct Host {{ unsigned state; struct Link {member}[2]; }};
""", path)

        views = (host("src/a.c", "u16 phase; u8 pad2[2];", "linked") |
                 host("src/b.c", "u16 phase; u16 target;", "linked") |
                 host("src/c.c", "u16 phase; u16 target;", "other"))
        parents = {f"src/{name}.c:struct Host" for name in "abc"}
        children = ["src/a.c:struct Link", "src/b.c:struct Link"]
        self.assertEqual(families.family_components(set(views), views, {}, defaultdict(set)), [])
        edges = defaultdict(set)
        groups = families.family_components(set(views), views, {("unit", "Host"): parents}, edges)
        self.assertEqual(groups, [sorted(parents), children])
        self.assertEqual({kind for kind, _ in edges[tuple(children)]}, {"member"})

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

    def test_nested_bitfield_flattens_without_touching_another_record(self):
        path = "src/a.c"
        old_text = """
struct Half { unsigned lo : 16; unsigned hi : 16; };
struct Packed { struct Half xy; unsigned z : 16; };
struct Other { struct Half xy; };
int Read(struct Packed *p, struct Other *q) { return p->xy.hi + q->xy.hi; }
"""
        old = measure(old_text, path)[f"{path}:struct Packed"]
        new = measure("struct Vector { unsigned x : 16; unsigned y : 16; unsigned z : 16; };",
                      "include/vector.h")["include/vector.h:struct Vector"]
        with patch.object(lm, "preprocess", return_value=parse(old_text, path)[0]):
            result = merge.rewrite_unit(old_text, old, new, Path("include/vector.h"), "struct Vector")
        self.assertIn("return p->y + q->xy.hi;", result)

    def test_literal_array_subscript_resolves_to_the_measured_scalar(self):
        path = "src/a.c"
        declaration = TYPES + "struct Raw { u16 v[3]; };\n"
        new = measure(TYPES + "struct Config { u16 kind, flags, speed; };",
                      "include/config.h")["include/config.h:struct Config"]
        for subscript, refusal in (("0x2U", None), ("i", "no unambiguous shared member"),
                                   ("1 + 1", "nonliteral source index")):
            source = declaration + f"void Set(struct Raw *r, int i) {{ r->v[{subscript}] = i; }}"
            old = measure(source, path)[f"{path}:struct Raw"]
            with self.subTest(subscript=subscript), patch.object(lm, "preprocess", return_value=parse(source, path)[0]):
                if refusal:
                    with self.assertRaisesRegex(merge.Holdout, refusal):
                        merge.rewrite_unit(source, old, new, Path("include/config.h"), "struct Config")
                else:
                    revised = merge.rewrite_unit(source, old, new, Path("include/config.h"), "struct Config")
                    self.assertIn("r->speed = i;", revised)

    def test_aligned_record_is_editable_and_keeps_its_alignment(self):
        path = "src/a.c"
        text = TYPES + "struct Short { u16 value; } __attribute__((aligned(4)));\n"
        old = measure(text, path)[f"{path}:struct Short"]
        family = {"kind": "struct", "size": old["size"], "loads": []}
        generated, expected, _, _ = merge.plan_header(family, [old], "Shared", Path("include/shared.h"))
        self.assertIn("__attribute__((aligned(4)))", generated)
        new = merge.probe_header(generated, "struct Shared", expected, 4, 4)
        with patch.object(lm, "preprocess", return_value=parse(text, path)[0]):
            result = merge.rewrite_unit(text, old, new, Path("include/shared.h"), "struct Shared")
        self.assertNotIn("struct Short", result)

    def test_unnamed_bitfield_does_not_block_a_measured_common_layout(self):
        one = "struct Half { unsigned lo : 16; unsigned : 16; };"
        two = "struct Half { unsigned lo : 16; unsigned hi : 16; };"
        views = [next(iter(measure(src, f"src/{i}.c").values())) for i, src in enumerate((one, two))]
        family = {"kind": "struct", "size": 4, "loads": []}
        generated, expected, _, _ = merge.plan_header(family, views, "Pair", Path("include/pair.h"))
        self.assertEqual(expected, {"lo": (0, 2, 0, 16), "hi": (2, 2, 16, 16)})
        merge.probe_header(generated, "struct Pair", expected, 4, 4)

    def test_joint_merge_retains_typedef_and_rewrites_references_between_definitions(self):
        path = "src/a.c"
        source = """
typedef struct Pair { unsigned count; } PairAlias;
struct Tail { unsigned index; };
int Read(struct Pair *pair, PairAlias *alias, struct Tail *tail) {
    return pair->count + alias->count + tail->index;
}
"""
        olds = measure(source, path)
        new = next(iter(measure("struct Slot { unsigned position; };", "include/slot.h").values()))
        with patch.object(lm, "preprocess", return_value=parse(source, path)[0]):
            text = merge.rewrite_records(source, [(old, new, "struct Slot") for old in olds.values()],
                                         Path("include/slot.h"))
        self.assertIn("typedef struct Slot PairAlias;", text)
        self.assertIn("Read(struct Slot *pair, PairAlias *alias, struct Slot *tail)", text)
        self.assertIn("return pair->position + alias->position + tail->position;", text)
        self.assertNotIn("struct Pair", text)
        self.assertNotIn("struct Tail", text)

    def test_pointer_members_only_require_a_private_tag_declaration(self):
        path = "src/a.c"
        source = """
struct Node { unsigned id; };
struct Host {
    struct Node *head;
    void (*visit)(struct Node *node);
    struct Host *next;
    struct Node (*batch)[2];
};
"""
        _, _, _, tu = parse(source, path)
        host = tu.tags["struct", "Host"]
        deps = [evidence.member_dependencies(tu, decl, "struct Host") for decl in host.decls]
        self.assertEqual(deps, [([], [], ["struct Node"], False), ([], [], ["struct Node"], False),
                                ([], [], [], True), ([], ["struct Node"], [], False)])
        old = measure(source, path)[f"{path}:struct Host"]
        old["members"] = old["members"][:3]
        old["size"] = 12
        for member, (headers, private, forwards, self_ref) in zip(old["members"], deps):
            member.update(headers=headers, private=private, forwards=forwards, self_ref=self_ref)
        family = {"kind": "struct", "size": 12, "loads": []}
        generated, expected, _, _ = merge.plan_header(family, [old], "List", Path("include/list.h"))
        self.assertIn("struct Node;", generated)
        self.assertIn("struct List *next;", generated)
        merge.probe_header(generated, "struct List", expected, 12, 4)

    def test_merge_lifts_repeated_private_elements_into_the_same_header(self):
        source = """
struct Entry { unsigned timer, state; };
struct Pool { unsigned count; struct Entry slots[4]; };
unsigned Read(struct Pool *pool) { return pool->slots[2].timer; }
"""
        records = measure(source, "src/a.c") | measure(source, "src/b.c")
        views = [v for v in records.values() if v["spell"] == "struct Pool"]
        family = {"kind": "struct", "size": 36, "loads": [], "name": "Pool"}
        args = SimpleNamespace(use_shared=None, name="ScenePool", header=Path("include/scene_pool.h"))
        header, spell, text, measured, _, _ = merge.header_choice({"records": records}, family, views, args)
        self.assertIn("struct Entry slots[4];", text)
        self.assertLess(text.index("struct Entry {"), text.index("struct ScenePool {"))
        self.assertEqual(set(measured["embedded"]), {"src/a.c:struct Entry", "src/b.c:struct Entry"})
        child = records["src/a.c:struct Entry"]
        targets = [(views[0], measured, spell), (child, measured["embedded"][child["id"]], child["spell"])]
        with patch.object(lm, "preprocess", return_value=parse(source, "src/a.c")[0]):
            rewritten = merge.rewrite_records(source, targets, header)
        self.assertIn("struct ScenePool *pool", rewritten)
        self.assertIn("pool->slots[2].timer", rewritten)
        self.assertNotIn("struct Entry {", rewritten)
        with self.assertRaisesRegex(merge.Holdout, "depends on struct Entry"):
            merge.header_choice({"records": records}, family, views[:1], args)

    def test_pointer_view_adopts_a_larger_type_only_without_extent_uses(self):
        path = "src/a.c"
        short = "struct Short { unsigned phase; };\n"
        safe = short + "void Step(struct Short *p) { p->phase = 1; }\n"
        new = next(iter(measure("struct Actor { unsigned state, timer; };", "include/actor.h").values()))
        for body, reason in ((safe, None),
                             (short + "unsigned Read(struct Short *p) { return sizeof(*p); }", "sizeof"),
                             (short + "void Step(struct Short *p) { p[1].phase = 1; }", "indexed pointer"),
                             (short + "void Copy(struct Short *p, struct Short *q) { *p = *q; }", "by-value assignment"),
                             (short + "struct Short items[2];", "by-value object")):
            with self.subTest(body=body):
                old = next(iter(measure(body, path).values()))
                targets = [(old, new, "struct Actor")]
                with patch.object(lm, "preprocess", return_value=parse(body, path)[0]):
                    if reason:
                        with self.assertRaisesRegex(merge.Holdout, reason):
                            merge.rewrite_records(body, targets, Path("include/actor.h"), allow_prefix=True)
                    else:
                        with self.assertRaisesRegex(merge.Holdout, "size/alignment"):
                            merge.rewrite_records(body, targets, Path("include/actor.h"))
                        rewrite = merge.rewrite_records(body, targets, Path("include/actor.h"), allow_prefix=True)
                        self.assertIn("void Step(struct Actor *p) { p->state = 1; }", rewrite)

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

    def test_whole_byte_bitfields_vote_by_storage_type_and_keep_positions(self):
        copies = [
            "unsigned count : 16; unsigned extra : 16;",
            "u32 count : 16; u32 extra : 16;",
            "u16 count; u16 extra;",
        ]
        views = [next(iter(measure(TYPES + "typedef unsigned u32; struct W { " + fields
                                   + " int *values; };", f"src/{i}.c").values()))
                 for i, fields in enumerate(copies)]
        family = {"kind": "struct", "size": views[0]["size"], "loads": []}
        generated, expected, _, _ = merge.plan_header(family, views, "W", Path("include/w.h"))
        self.assertEqual(expected, {"count": (0, 2, 0, 16), "extra": (2, 2, 16, 16), "values": (4, 4)})
        measured = merge.probe_header(generated, "struct W", expected, views[0]["size"], views[0]["align"])
        self.assertEqual(merge.field_paths(views[2], measured), {"count": "count", "extra": "extra", "values": "values"})
        loads = [(0, 2, False, "Halfword", 0x8000010), (0, 4, None, "Word", 0x8000020),
                 (0, 2, True, "Signed", 0x8000030)]
        self.assertEqual(merge.load_vote(views[0], views[0]["members"][0], loads), {(0, "Halfword")})
        with self.assertRaisesRegex(merge.Holdout, "generated extra measured"):
            merge.probe_header(generated, "struct W", {**expected, "extra": (2, 2, 17, 15)},
                               views[0]["size"], views[0]["align"])

    def test_sub_byte_bitfield_rewrite_requires_the_same_bits(self):
        old = next(iter(measure("struct W { unsigned low : 4; unsigned high : 4; };", "src/a.c").values()))
        new = next(iter(measure("struct W { unsigned whole : 8; };", "include/w.h").values()))
        family = {"kind": "struct", "size": old["size"], "loads": []}
        with self.assertRaisesRegex(merge.Holdout, "allocation-group"):
            merge.plan_header(family, [old], "W", Path("include/w.h"))
        self.assertEqual(merge.field_paths(old, new), {"low": None, "high": None})


class GateTests(unittest.TestCase):
    @unittest.skipUnless(sys.platform == "darwin" and Path(deadcode.CLANG).is_file(), "macOS clang required")
    def test_direct_preprocessor_preserves_diagnostic_input(self):
        source = "#define OFFSET 4\nint field = OFFSET;\n"
        args = ["-nostdinc", "-undef", "-x", "c", "-"]
        expected = subprocess.check_output(["cc", "-E", *args], input=source, text=True)
        self.assertEqual(subprocess.check_output([*deadcode.CPP, *args], input=source, text=True), expected)

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
