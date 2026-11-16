"""retype_fields rewrites the member accesses the compiler rejects, and only those."""
import os
import shutil
import sys
import tempfile
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "tools"))

import retype_fields as rf  # noqa: E402

PAIRS = {"health": "base.health", "body": "base.body", "word104": "savedControl"}


class Lines(unittest.TestCase):
    def test_moves_into_base(self):
        text, bad = rf.rewrite_line("a->health = b->body.x;", {"health": 1, "body": 1}, PAIRS)
        self.assertEqual(text, "a->base.health = b->base.body.x;")
        self.assertEqual(bad, [])

    def test_skips_already_rewritten(self):
        text, _ = rf.rewrite_line("a->base.health = b->health;", {"health": 1}, PAIRS)
        self.assertEqual(text, "a->base.health = b->base.health;")

    def test_second_level(self):
        # a class whose own `base` embeds the rewritten struct
        text, _ = rf.rewrite_line("x = a->base.health;", {"health": 1}, PAIRS)
        self.assertEqual(text, "x = a->base.base.health;")

    def test_rename(self):
        text, _ = rf.rewrite_line("p . word104 = 0;", {"word104": 1}, PAIRS)
        self.assertEqual(text, "p . savedControl = 0;")

    def test_ambiguous_left_alone(self):
        line = "save->health = actor->health;"
        text, bad = rf.rewrite_line(line, {"health": 1}, PAIRS)
        self.assertEqual(text, line)
        self.assertEqual(len(bad), 1)

    def test_macro_argument_expanded_twice(self):
        text, _ = rf.rewrite_line("SET(out, actor->body.motion, 2);", {"body": 2}, PAIRS)
        self.assertEqual(text, "SET(out, actor->base.body.motion, 2);")

    def test_call_arguments(self):
        s = "F(a(b, c), d[1], e)"
        self.assertEqual([s[a:b] for a, b in rf.call_arguments(s, 1)], ["a(b, c)", " d[1]", " e"])


class Steps(unittest.TestCase):
    def test_room_actor_steps_parse(self):
        steps = rf.read_steps(os.path.join(ROOT, "tools", "retype", "room_actor.txt"))
        kinds = [k for k, _, _ in steps]
        self.assertEqual(kinds[-1], "embed")  # the embedding runs after every rename
        self.assertEqual(steps[-1][1], ["RoomActor", "base"])
        self.assertIn(("types", [], {"SceneActorAnimationView": "RoomActor"}), steps)

    def test_type_rename(self):
        d = tempfile.mkdtemp(dir=os.path.join(ROOT, "build"))
        try:
            path = os.path.join(d, "t.c")
            with open(path, "w") as f:
                f.write("struct OldView *a; struct OldViewer *b;\n")
            rf.replace_text([os.path.relpath(path, ROOT)], {"OldView": "RoomActor"}, regex=True)
            with open(path) as f:
                self.assertEqual(f.read(), "struct RoomActor *a; struct OldViewer *b;\n")
        finally:
            shutil.rmtree(d)


@unittest.skipUnless(os.path.exists(os.path.join(ROOT, "build", "tools", "agbcc", "agbcc")), "needs agbcc")
class EndToEnd(unittest.TestCase):
    """A unit and a header in a scratch copy of include/: macros, offsetof-style
    arguments, a header line, and an access that already compiles."""

    def setUp(self):
        self.dir = tempfile.mkdtemp(dir=os.path.join(ROOT, "build"))
        self.rel = os.path.relpath(self.dir, ROOT)
        with open(os.path.join(self.dir, "probe.h"), "w") as f:
            f.write("struct Base { int health; int body; };\n"
                    "struct Thing { struct Base base; int own; };\n"
                    "static inline int Thing_Health(struct Thing *t) { return t->health; }\n")
        with open(os.path.join(self.dir, "probe.c"), "w") as f:
            f.write('#include "global.h"\n'
                    f'#include "{self.rel}/probe.h"\n'
                    "#define HURT(t) ((t)->health--)\n"
                    "#define OFFSET(m) ((unsigned)&((struct Thing *)0)->m)\n"
                    "int f(struct Thing *t, struct Base *b)\n"
                    "{\n"
                    "    HURT(t);\n"
                    "    b->health = t->own + OFFSET(body);\n"
                    "    return t->health + t->body;\n"
                    "}\n")

    def tearDown(self):
        shutil.rmtree(self.dir)

    def test_run(self):
        unit = os.path.join(self.rel, "probe.c")
        _, unsettled, failed = rf.run([unit], {"health": "base.health", "body": "base.body"}, 1)
        self.assertEqual((unsettled, failed), ({}, {}))
        with open(os.path.join(self.dir, "probe.c")) as f:
            text = f.read()
        self.assertIn("#define HURT(t) ((t)->base.health--)", text)
        self.assertIn("b->health = t->own + OFFSET(base.body);", text)
        self.assertIn("return t->base.health + t->base.body;", text)
        with open(os.path.join(self.dir, "probe.h")) as f:
            self.assertIn("return t->base.health;", f.read())
        self.assertEqual(rf.compile_errors(unit).strip(), "")


if __name__ == "__main__":
    unittest.main()
