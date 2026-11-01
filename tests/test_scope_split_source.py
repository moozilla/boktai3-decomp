"""fast.split_source cuts the unit at FUNC's definition, never at a call that looks like one."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools" / "scope"))
import fast  # noqa: E402

SOURCE = """int Target(int a, int b)
{
    return a + b;
}

void Later(int *p)
{
    if (p[0]) {
        p[1] = 0;
    } else if (!Target(p[2], p[3])) {
        p[4] = 1;
    }
    p[5] = 2;
}
"""


class SplitSource(unittest.TestCase):
    def test_a_call_in_a_later_function_does_not_extend_the_cut(self):
        prefix, body = fast.split_source(SOURCE, ["Target"])
        self.assertTrue(body.rstrip().endswith("return a + b;\n}"))
        self.assertNotIn("Later", body)


if __name__ == "__main__":
    unittest.main()
