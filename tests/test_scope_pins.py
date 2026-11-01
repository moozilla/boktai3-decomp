"""`scope.py pins` considers only concessions that can change the function's code, and groups them."""
from pathlib import Path
import unittest

from tools.matchsearch.evaluator import _load_scope

SOURCE = """struct S { int a; };
static inline int Helper(int v)
{
    register int t asm("r1") = v;
    asm("" : "+r"(t));
    return t + 1;
}
static inline int Unused(int v)
{
    register int u asm("r2") = v;
    return u;
}
int Other(int v)
{
    register int o asm("r3") = v;
    return Helper(o);
}
int Target(struct S *s)
{
    register int x asm("r4") = s->a;
    return Helper(x);
}
#ifdef NONMATCHING
int Draft(int v)
{
    register int d asm("r5") = v;
    return d;
}
#endif
"""


class PinsScope(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scope, _ = _load_scope(Path(__file__).resolve().parents[1])

    def test_reach_is_the_function_and_its_inline_helpers(self):
        reach = self.scope.function_reach(SOURCE, "Target")
        self.assertEqual(set(reach), {"Target", "Helper"})
        self.assertEqual(set(self.scope.function_reach(SOURCE, "Other")), {"Other", "Helper"})

    def test_drafts_are_left_out(self):
        self.assertEqual(self.scope.function_reach(SOURCE, "Draft"), {})

    def test_sites_outside_the_reach_are_not_offered(self):
        reach = self.scope.function_reach(SOURCE, "Target")
        lines = SOURCE.split("\n")
        starts = [0]
        for ln in lines:
            starts.append(starts[-1] + len(ln) + 1)
        sites = [i for i, _, _ in self.scope.concession_sites(SOURCE)
                 if any(a <= starts[i] < b for a, b in reach.values())]
        self.assertEqual([lines[i].strip() for i in sites],
                         ['register int t asm("r1") = v;', 'asm("" : "+r"(t));', 'register int x asm("r4") = s->a;'])

    def test_variables_group_a_pin_with_its_barriers(self):
        self.assertEqual(self.scope.site_variables('    register int t asm("r1") = v;'), {"t"})
        self.assertEqual(self.scope.site_variables('    asm("" : "+r"(t), "=r"(u));'), {"t", "u"})
        self.assertEqual(self.scope.site_variables('    asm("" ::: "memory");'), set())


if __name__ == "__main__":
    unittest.main()
