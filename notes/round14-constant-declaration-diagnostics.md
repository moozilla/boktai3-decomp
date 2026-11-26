# Round 14: distinguish declarations from jumps and returns

Tags: round14, tooling, concessions, const-local

Date: 2026-11-16. Source snapshot: `9b40f4fdd`.

The `const-local` spelling detector accepted any word as a type name. That
included C's `goto` and `return` keywords, producing an additional warning
for ordinary control flow rather than a declaration. Three concrete examples:

| Source | Statement | Actual role |
|---|---|---|
| `src/monsters/axe/axe.c:2248` | `goto zero;` | Branch to the helper's false-result label. |
| `src/monsters/spider.c:40` | `goto one;` | Branch to the helper's true-result label. |
| `src/effects/particles/frost_emitter.c:186` | `return zero;` | Read an existing local; its declaration is reported separately. |

Exclude only those two reserved control-flow keywords from the type position.
Arbitrary typedef names remain recognized. A real `zero`/`one` declaration
remains reported when the function later returns that variable. This does not
change the policy on constant storage, labels, gotos, or allocation-only C.

On the same tree, `const-local` falls from **1,519 to 1,400**. Complete totals
change from **3 errors / 2,043 warnings** to **3 errors / 1,924 warnings**.
Every other kind is unchanged. These **119 false reports** are a detector
correction, not source removals. No source file or ignore comment changes.

All 33 concession tests pass. The added regression checks both jump labels,
both returned constants, a dereferenced return operand, and actual declarations
using a typedef or unsigned type. The ignored audit record
`build/round14/keyword-removed-sites.json` lists every removed report.
