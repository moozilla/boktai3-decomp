"""layoutmerge.strip_bodies keeps declarations and drops bodies and initializers."""
import os
import sys
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "tools"))

import layoutmerge  # noqa: E402


class StripBodies(unittest.TestCase):
    def test_size_check_bound_is_kept(self):
        # `==` inside an array bound is a comparison, not an initializer.
        t = "struct T { int a; };\ntypedef char C[sizeof(struct T) == 4 ? 1 : -1];\nint x;\n"
        self.assertEqual(layoutmerge.strip_bodies(t), t)

    def test_initializer_dropped(self):
        out = layoutmerge.strip_bodies("int a[2] = {1, 2};\nint b;\n")
        self.assertTrue(out.startswith("int a[2] "))
        self.assertNotIn("{", out)
        self.assertIn("int b;", out)

    def test_function_body_dropped(self):
        out = layoutmerge.strip_bodies("void f(void) { int x = 1; }\n")
        self.assertEqual(out.split()[:3], ["void", "f(void)", ";"])
        self.assertNotIn("x", out)


if __name__ == "__main__":
    unittest.main()
