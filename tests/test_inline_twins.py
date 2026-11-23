"""tools/inline_twins.py: which private helpers and structs are exact twins of a shared one, and
how a replacement edits the unit (the keep-or-revert gate is tools/keepcheck.py's)."""
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "tools"))
import inline_twins as it  # noqa: E402

HEADER = """\
static inline void Chase_SetState(struct Actor *actor, u8 state, u8 phase)
{
    actor->state = state;
    actor->phase = phase; // the phase
    actor->timer = 0x10;
}

static inline u32 Flags_Test(u32 *flags, u32 mask)
{
    return *flags & mask;
}

struct Vec {
    s16 x, y, z;
};

struct Pair {
    s32 a;
    s32 b;
};
"""

UNIT = """\
#include "global.h"

static inline void Crow_SetState(struct Actor *a, u8 s, u8 p)
{
    a->state = s;
    a->phase = p;
    a->timer = 16;
}

// tests the mask
static inline u32 Crow_Test(u32 mask, u32 *flags)
{
    return *flags & mask;
}

struct Vec {
    s16 x, y, z; /* 0x00 */
};

struct Coord {
    s32 a;
    s32 b;
};

void Crow_Update(struct Actor *a, struct Vec *v)
{
    Crow_SetState(a, 1, 2);
    if (Crow_Test(4, &a->flags))
        a->x = 1;
}
"""


def index(text, name="shared.h"):
    hk, hl, rk = {}, {}, {}
    for h in it.helpers(text, name):
        hk.setdefault(h.key, []).append(h)
        hl.setdefault(h.loose, []).append(h)
    for r in it.records(text, name):
        rk.setdefault((r.kind, r.typedef, r.body), []).append(r)
    return hk, hl, rk


class Twins(unittest.TestCase):
    def setUp(self):
        self.idx = index(HEADER)

    def test_exact_helper_twin_ignores_names_whitespace_comments_and_number_spelling(self):
        exact, near = it.plan(UNIT, self.idx, [])
        helpers = [(p.name, [c.name for c in cands]) for kind, p, cands in exact if kind == "helper"]
        self.assertEqual(helpers, [("Crow_SetState", ["Chase_SetState"])])

    def test_parameter_order_is_a_near_twin(self):
        exact, near = it.plan(UNIT, self.idx, [])
        self.assertIn(("helper", "Crow_Test"), [(k, p.name) for k, p, _ in near])
        self.assertNotIn("Crow_Test", [p.name for _, p, _ in exact])

    def test_struct_twin_needs_the_same_name(self):
        exact, near = it.plan(UNIT, self.idx, [])
        self.assertIn(("record", "Vec"), [(k, p.name) for k, p, _ in exact])
        # same members, another name: reported only
        self.assertIn(("record", "Coord"), [(k, p.name) for k, p, _ in near])
        self.assertNotIn("Coord", [p.name for _, p, _ in exact])

    def test_another_body_is_no_twin(self):
        other = UNIT.replace("a->timer = 16;", "a->timer = 17;")
        exact, _ = it.plan(other, self.idx, [])
        self.assertNotIn("Crow_SetState", [p.name for _, p, _ in exact])

    def test_drafts_are_left_alone(self):
        start = UNIT.index("static inline void Crow_SetState")
        exact, _ = it.plan(UNIT, self.idx, [(start, start + 10)])
        self.assertNotIn("Crow_SetState", [p.name for _, p, _ in exact])

    def test_replace_drops_the_definition_and_renames_calls(self):
        exact, _ = it.plan(UNIT, self.idx, [])
        kind, priv, cands = next(e for e in exact if e[1].name == "Crow_SetState")
        new = it.replace(UNIT, kind, priv, cands[0])
        self.assertNotIn("Crow_SetState", new)
        self.assertIn("    Chase_SetState(a, 1, 2);", new)
        self.assertNotIn("\n\n\n", new)

    def test_replace_takes_the_comment_above(self):
        idx = index(HEADER.replace("u32 *flags, u32 mask", "u32 mask, u32 *flags"))
        exact, _ = it.plan(UNIT, idx, [])
        kind, priv, cands = next(e for e in exact if e[1].name == "Crow_Test")
        new = it.replace(UNIT, kind, priv, cands[0])
        self.assertNotIn("tests the mask", new)
        self.assertIn("Flags_Test(4, &a->flags)", new)

    def test_record_rename_touches_only_the_tag(self):
        text = "struct Old {\n    s32 a;\n};\nstruct Old *p; int Old;\n"
        priv = it.records(text)[0]
        shared = it.Record("struct", "New", False, priv.body, 0, 0, "x.h")
        self.assertEqual(it.replace(text, "record", priv, shared), "struct New *p; int Old;\n")

    def test_member_named_like_the_helper_is_not_renamed(self):
        self.assertEqual(it.rename("f(x); p->f = 1; s.f = 2; /* f(x) */", "f", "g"),
                         "g(x); p->f = 1; s.f = 2; /* f(x) */")

    def test_add_include_goes_after_the_leading_block(self):
        text = '#include "a.h"\n/* note */\n#include "b.h"\n\nint x;\n#ifdef NONMATCHING\n#include "c.h"\n#endif\n'
        self.assertEqual(it.add_include(text, "new.h"),
                         '#include "a.h"\n/* note */\n#include "b.h"\n#include "new.h"\n\nint x;\n'
                         '#ifdef NONMATCHING\n#include "c.h"\n#endif\n')

    def test_generic_names(self):
        self.assertTrue(it.generic("scale"))
        self.assertFalse(it.generic("Chase_Delta"))

    def test_typedef_records(self):
        rs = it.records("typedef struct {\n    u8 a;\n} Thing;\ntypedef struct T {\n    u8 a;\n} U;\n")
        self.assertEqual([(r.name, r.typedef) for r in rs], [("Thing", True)])


if __name__ == "__main__":
    unittest.main()
