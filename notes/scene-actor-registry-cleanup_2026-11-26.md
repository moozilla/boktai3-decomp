# Scene actor registry cleanup

Tags: #matching #types #phase3 #scene

Registry diagnostics fell from 111 lines to 104 (6.3%). Charged constant uses
fell from 128 to 120 (6.25%), with zero errors. Code coverage remains 100%,
a change of zero percentage points. The stock compiler matches every changed
translation unit.

- The lookups and record tick (0806F9AC, 0806F9D8, 0806FA14) now use
  ordinary indexed loops over the existing record type. In the pair lookup,
  initializing the index before capturing the table preserves the two
  register homes. Scope isolates the original discrepancy in CSE's choice
  of pointer equivalence and the position of the zero load.
- The movement mode (08070070) uses the native player and chase route.
  The registry update (08070FA8) uses AnimSprite flags and palette bank.
- Scripted defeat (080704EC) uses the shared guarded record. The two route
  resets (080714E4, 0807171C) use the shared interaction cleanup, typed
  sprite fields, and the existing terrain height operation where needed.
- The four generic word conversions (0806FB00, 08070444, 080706DC,
  08070E44) return to explicit local masks because the direct versions
  differ. The count and find paths use the native status or behavior
  field where it preserves the original instructions. These reversions
  leave their charged counts unchanged.

The direct palette-offset criterion in 08070444 and the direct status-field
expressions in its three living-query siblings change the address computation
across the loop. In 08070FA8 the direct GameState read differs in seven
register-only instructions because the singleton address moves from r0 to r1.
Direct point vectors and ordinary loop control also change allocation. The
original exact forms remain in those regions.
