"""`why` lists the gotos before a reload-rotation difference (tails cross-jumping merges after reload)."""
from pathlib import Path
import tempfile
import unittest

from tools.matchsearch.evaluator import _load_scope

UI = """# 1 "draft.c"
# 1 "include/global.h" 1
int header_line; goto nowhere;
# 2 "draft.c" 2
void f(int x)
{
    if (x)
        goto done;
    g();
done:
    h();
    goto again;
}
"""


class GotoHint(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def setUp(self):
        self.dir = tempfile.TemporaryDirectory()
        Path(self.dir.name, "u.i").write_text(UI)

    def tearDown(self):
        self.dir.cleanup()

    def test_source_lines_skip_headers(self):
        src = self.scope.source_lines(self.dir.name)
        self.assertEqual(src[2], "void f(int x)")
        self.assertEqual(src[5], "        goto done;")
        self.assertNotIn("header_line", " ".join(src.values()))

    def test_gotos_before_the_insn_line_only(self):
        lines = {10: 4, 11: 5, 12: 6, 13: 8, 14: 9}
        hits = self.scope.gotos_before(self.dir.name, "f", 13, lines=lines)
        self.assertEqual(hits, [(5, "goto done;")])
        self.assertEqual(self.scope.gotos_before(self.dir.name, "f", 11, lines=lines), [])
        self.assertEqual(self.scope.gotos_before(self.dir.name, "f", 99, lines=lines), [])


if __name__ == "__main__":
    unittest.main()
