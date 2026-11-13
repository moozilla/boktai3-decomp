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

    def test_symbol_reaches_a_file_local_alias_without_unrelated_functions(self):
        source = '#define Target sub_081FDCB0 // provisional name\n' + SOURCE
        reach = self.scope.function_reach(source, "sub_081FDCB0")
        self.assertEqual(set(reach), {"Target", "Helper"})
        self.assertEqual(reach, self.scope.function_reach(source, "Target"))
        a, b = reach["Target"]
        self.assertIn('asm("r4")', source[a:b])
        self.assertNotIn('asm("r3")', source[a:b])

    def test_alias_chains_and_inline_helper_calls_are_resolved(self):
        source = ("#define Target Middle\n#define Middle sub_081FDCB0\n"
                  "#define AliasHelper Helper /* shared inline */\n"
                  + SOURCE.replace("return Helper(x);", "return AliasHelper(x);"))
        self.assertEqual(set(self.scope.function_reach(source, "sub_081FDCB0")),
                         {"Target", "Helper", "#AliasHelper"})

    def test_aliases_in_nonmatching_drafts_are_ignored(self):
        source = "#ifdef NONMATCHING\n#define Target sub_081FDCB0\n#endif\n" + SOURCE
        self.assertEqual(self.scope.function_reach(source, "sub_081FDCB0"), {})

    def test_cyclic_aliases_terminate(self):
        source = "#define Missing Cycle\n#define Cycle Missing\n" + SOURCE
        self.assertEqual(self.scope.function_reach(source, "Missing"), {})

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

    def test_a_function_after_a_continued_define_is_found_with_the_macros_it_uses(self):
        source = """#define PINNED(v)                                                   \\
    ({                                                                \\
        register int m asm("r1") = (v);                               \\
        m;                                                            \\
    })
#define UNUSED(v) ({ register int n asm("r2") = (v); n; })
int Target(int v)
{
    return PINNED(v) + 1;
}
"""
        reach = self.scope.function_reach(source, "Target")
        self.assertEqual(set(reach), {"Target", "#PINNED"})
        a, b = reach["#PINNED"]
        self.assertIn('asm("r1")', source[a:b])

    def test_a_barrier_line_inside_a_macro_is_a_site(self):
        source = "#define KEEP(v)  \\\n    ({  \\\n        asm(\"\" : \"+r\"(v));  \\\n        v;  \\\n    })\n"
        kinds = [k for _, k, _ in self.scope.concession_sites(source)]
        self.assertEqual(kinds, ["barrier"])

    def test_memory_barrier_with_trailing_block_comment_is_a_site(self):
        source = 'void Target(void)\n{\n    asm("" ::: "memory"); /* preserve the reload */\n}\n'
        sites = self.scope.concession_sites(source)
        self.assertEqual([(i, kind) for i, kind, _ in sites], [(2, "barrier")])
        self.assertEqual(sites[0][2](source.split("\n")), ['void Target(void)', '{', '}', ''])

    def test_barrier_trailing_comment_does_not_hide_following_code(self):
        source = '    asm("" ::: "memory"); /* comment */ value++;\n'
        self.assertEqual(self.scope.concession_sites(source), [])

    def test_each_pin_of_a_declaration_line_is_a_site(self):
        source = 'int F(void)\n{\n    register int a asm("r9"), b asm("r6"), c;\n    return a + b;\n}\n'
        sites = self.scope.concession_sites(source)
        self.assertEqual(len(sites), 2)
        lines = source.split("\n")
        self.assertEqual(sites[0][2](lines)[2].strip(), 'register int a, b asm("r6"), c;')
        self.assertEqual(sites[1][2](lines)[2].strip(), 'register int a asm("r9"), b, c;')
        both = sites[1][2](sites[0][2](lines))
        self.assertEqual(both[2].strip(), "register int a, b, c;")

    def test_other_concessions_include_reached_volatile_locals(self):
        source = SOURCE.replace('register int t asm("r1") = v;',
                                'volatile int t = v;').replace(
                                    'register int x asm("r4") = s->a;',
                                    'struct S *volatile copy = s;\n    int x = copy->a;')
        reach = self.scope.function_reach(source, "Target")
        found = self.scope.unprobed_concessions(source, reach)
        self.assertEqual([(kind, owner) for _, kind, owner in found],
                         [("volatile", "Helper"), ("volatile", "Target")])
        lines = source.splitlines()
        self.assertIn('volatile int t', lines[found[0][0] - 1])
        self.assertIn('*volatile copy', lines[found[1][0] - 1])

    def test_other_concessions_exclude_unreached_helpers_and_comments(self):
        source = SOURCE.replace('register int u asm("r2") = v;', 'volatile int u = v;')
        source = source.replace('return Helper(x);',
                                '/* volatile int ignored; */\n    return Helper(x);')
        self.assertEqual(self.scope.unprobed_concessions(
            source, self.scope.function_reach(source, "Target")), [])

    def test_volatile_pointee_is_not_a_volatile_local(self):
        source = 'int Target(volatile int *reg)\n{\n    volatile int *p = reg;\n    return *p;\n}\n'
        self.assertEqual(self.scope.unprobed_concessions(
            source, self.scope.function_reach(source, "Target")), [])


if __name__ == "__main__":
    unittest.main()
