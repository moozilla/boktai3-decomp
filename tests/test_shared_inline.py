"""tools/shared_inline.py: a private, pinned copy of a header's inline helper is found."""
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import shared_inline as si  # noqa: E402

SHARED = """static inline void Seq_Step(struct Sprite *sprite, struct State *state)
{
    const u16 *command = state->commands + state->index;
    sprite->frame = *command >> 6;
    if ((state->flags & 1) != (((*command & 0x30) >> 4) & 1))
        sprite->flags |= 4;
    else
        sprite->flags &= ~4;
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        state->index++;
        state->duration = (state->weight * state->speed) >> 6;
    }
}
"""
PRIVATE = """inline static void My_Step(struct Mine *m, struct Seq *s)
{
    register u32 bit asm("r3");
    const volatile u16 *c = s->cmds + s->i;
    m->frame = *c >> 6;
    bit = 1;
    asm("" : "+r"(bit));
    if ((s->flags & bit) != (((*c & 0x30) >> 4) & bit))
        m->flags |= 4;
    else
        m->flags &= ~4;
    if (++s->elapsed >= s->duration) {
        s->elapsed = 0;
        s->i++;
        s->duration = (s->weight * s->speed) >> 6;
    }
}
void F(struct Mine *m) { My_Step(m, (struct Seq *)(m + 1)); }
"""


class SharedInline(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        (Path(self.tmp.name) / "include").mkdir()
        (Path(self.tmp.name) / "include" / "seq.h").write_text(SHARED)
        si.shared.cache_clear()

    def tearDown(self):
        self.tmp.cleanup()
        si.shared.cache_clear()

    def test_pinned_copy_matches_the_header(self):
        found = si.matches(PRIVATE, root=self.tmp.name)
        self.assertEqual(found[0][0], "My_Step")
        score, header, name = found[0][1][0]
        self.assertEqual((header, name), ("include/seq.h", "Seq_Step"))
        self.assertGreater(score, 0.8)

    def test_bodies_handles_both_orders_of_static_inline(self):
        self.assertEqual(set(si.bodies(PRIVATE + SHARED)), {"My_Step", "Seq_Step"})

    def test_unrelated_helper_is_quiet(self):
        text = "static inline u32 Sum(u32 *a)\n{\n" + "    a[0] += a[1] * a[2] - a[3];\n" * 8 + "    return a[0];\n}\n"
        self.assertEqual(si.matches(text, root=self.tmp.name), [])


if __name__ == "__main__":
    unittest.main()
