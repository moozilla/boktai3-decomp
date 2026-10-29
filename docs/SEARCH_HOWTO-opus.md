# seek: matching as a search, with you as the policy (worker guide)

`tools/seek/seek.py` keeps a search tree of programs for one function. Each node is a whole draft
(content-addressed), compiled through scope's fork server in about 10 ms and evaluated in layers.
You write the C that needs judgment: a struct, a macro, a loop the way a person writes it. The tool
tries the mechanical variants around it, remembers everything that was tried, and lets you back out
of a dead end. Sessions are shared by every worktree (`$SEEK_HOME`, default `~/git/boktai3-decomp/build/seek`),
so the next worker on a function starts where you stopped.

## The loop

```
python3 tools/seek/seek.py open src/fn/sub_XXXXXXXX.c      # or FILE.c FUNC for a multi-function unit
python3 tools/seek/seek.py facts                           # the determined part: calls, accesses, literals, frame
python3 tools/seek/seek.py run --seconds 60                # automatic best-first search from the best nodes
python3 tools/seek/seek.py tree                            # what the session holds, best first
python3 tools/seek/seek.py show n42                        # one node: layers, block map, C diff from its parent
cp build/seek/FUNC/n42.c mine.c; $EDITOR mine.c            # your hypothesis, as a normal C edit
python3 tools/seek/seek.py add mine.c -p n42 -m "u16 index: ROM uses lsls/lsrs"   # evaluated + expanded
python3 tools/seek/seek.py land n57                        # a MATCH node: campaign.py land does the merge
```

Every node file (`seek.py file N` prints its path) is an ordinary draft: run `scope.py FILE why all`,
`probe`, `life`, `values`, knobs, on it as before. seek does not replace scope's explanations; it holds
the search that used to live only in your head and the current best file.

## Reading a node

`n42  skel 0 shape 4 diff 9   5 ROM / 4 draft  (+0,-2,-3)  swap_stmts: swap statements 3/4 @41  [alloc 2 off] {i r4->r5}`

* **skel**: determined facts the draft misses: a call, a memory access (width, signedness, offset),
  a literal, the frame (pushed registers, stack size). Fix these first, by hand: they mean the C
  computes something else (wrong field, wrong type), and no reordering or allocation change will
  fix them. `facts` lists them.
* **shape**: register-blind distance (order and instruction choice). **diff**: differing insns.
  The search ranks by (skel, shape, diff): structure before registers, so a change that buys
  registers by breaking shape never wins.
* **(+0,-2,-3)**: change from the parent. **@41**: the C line the move touched.
* **blocks** (in `show`/`open`): one character per ROM basic block. `=` exact, `r` the same code up
  to a consistent renaming of registers inside the block (that block's C is right; the registers
  come from elsewhere), `s` same shape, `x` different. Work on the `x` blocks; `r` blocks are
  allocation, which is global.
* **[alloc N off]**: named values (C variables, `=N` constants) in another register than the ROM's
  for them; `{i r4->r5}` which values this move moved. From agbcc's own allocation records, per
  compile. `values N` prints the full table with priority, refs and live length and points at
  `global.c` (allocno_compare, find_reg) and `local-alloc.c` (qty_compare_1).

## Steering

* `expand N` tries every move once at N and prints which move families change the code at all
  ("no effect here" families are safe to ignore for this function) and the best children.
* `run [N...] --seconds S --budget C` best-first: expands the best unexpanded node, keeps children
  that improve or tie (a plateau is walked: equal-score code is new code), and never compiles the same
  output twice (one node per distinct binary). Restrict it: `--fam swap_stmts,decl_order,width`,
  `--lines 40-60` (only moves touching those source lines). `seek.py moves` lists the 35 families.
* `SEEK_JOBS=4` spreads each expansion over four processes, each with its own agbcc fork server
  (about 6 ms per variant instead of 16 on a 200-byte function). Be fair to other workers.
* Moves apply to the function and to the `static inline` helpers it calls.
* Moves rewrite through pycparser, so a move node's C has macros expanded and is regenerated in
  one style. Take the idea, not the text: port the change into your own file and `add` it.
* `note N TEXT` records a finding on a node for the next worker (`tree` shows notes).
* `[sem]` moves (casts, temporaries for repeated expressions) can change meaning; seek runs
  semcheck on them when unicorn is installed (`build/venv/bin/pip install unicorn`), else they
  are marked unchecked. The ROM match is the final check either way.

## What the search is good at, and what it is not

Good: allocation ties, declaration order, statement order, operand order, temporaries, widths, loop
and comparison forms: the finite edits that used to take a model dozens of hand compiles. Run it after
every real hypothesis of yours; it costs seconds.

Not: the shape the C is missing (a struct, a macro, an inline helper, a different loop). That is your
job. When `run` stalls with the same `x` blocks, read the ROM for those blocks (`scope.py rom FUNC`),
write the C a programmer would have written, and `add` it, even when it scores worse at first: the
tree keeps the old best, and the search will expand around your new idea.

## Design notes (where this departs from docs/SEARCH_SPEC.md, and why)

* **One node per distinct binary, not per program.** The space of C programs is infinite, but the
  space of outputs a neighborhood reaches is small: on the drafts tried, about half of all moves
  changed no byte, and many of the rest reproduced a binary already in the tree.
  Deduplicating by output (a transposition table) turns the tree into a graph over binaries and
  tells you, per family, which kinds of edit move this function at all.
* **The determined part is checked, not lifted.** Instead of a lifted value graph, seek extracts
  the facts C cannot vary (calls, accesses, literals, frame) from both listings and ranks by them
  first. That is the "solve the determined part first" gate at the cost of one regex pass per compile.
* **Value webs come from the compiler, per compile.** The instrumented agbcc already dumps each
  value's priority and register with its C name (AGBCC_DUMPPRI); seek passes it to the fork server's
  child, so every variant carries its allocation without a -g compile, and `[alloc N off]` counts
  values off the ROM's registers (voted once per file node).
* **Block verdicts up to renaming** answer "is this region's C right?" without being confounded by
  register shifts from elsewhere; they replace solving regions in isolation, which agbcc's global
  allocation makes unreliable.
* **Not built yet:** semantic pruning needs unicorn in build/venv (`[sem]` moves are unchecked
  without it); region-local search by ROM block (only `--lines`); reload rotation as an objective
  (the two nearest misses tried, GridAttackActor_OnDamageCollision and ActorStatus_InitializeContact,
  are the same `ldrsh` index reload, `AGBCC_SPILL=3:2` matches both).
