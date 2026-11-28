# Review baseline and the naturalize pilot (2026-11-27)

Tags: #process #phase3 #tooling #matching

Four measurements taken in one day, and the change of method they point to. The short
version: the linter's warning count was the wrong target, working one flagged constant at
a time was the wrong unit, and rewriting a whole function from what it does is cheap.

## 1. What the linter does not see

A reviewer (Sonnet, blind to the linter, rubric in `tools/review/RUBRIC.md`) judged 150
functions drawn at random from `src/` (not `src/lib`) on one question: would a professional
game programmer in 2005 plausibly have written this function this way?

| Verdict | Functions | With a linter finding | Lines |
|---|---|---|---|
| natural | 68 | 4 | 730 |
| minor (missing types, bare numbers, names) | 38 | 3 | 741 |
| contrived (shaped for the compiler) | 44 | 19 | 2,290 |

* 25 of the 44 contrived functions have no linter finding. Zero warnings would leave them.
* Contrived functions average 52 lines, natural ones 11: about 60% of the sampled lines.
* Scaled to the tree: roughly 3,000 contrived functions of about 10,700, on a sample of 150.
* Tags, most common first: cast-noise 37, decl-order 31, magic-numbers 29, control-flow 25,
  parked-constant 20, pointer-temp 18, value-reuse 13, naming 11, odd-helper 5,
  union-or-alias 3. The parked constant the linter charges is one kind among ten.

## 2. Plain C at the charged sites (`tools/plainprobe.py`)

Of 5,137 constant sites tested one at a time, plain C (`p->flags |= 4`) matches at 312
(6.1%); 2,665 fail with a single-use constant, 2,160 with a shared one. Applying the 312 by
tool was tried and discarded: it replaces readable accessor calls with forms like
`(((struct CameraTracking *)(&p->camera))->requests &= ~1);` and the linter rewards it. A
typed accessor named for its object is ordinary C; the linter should not charge it.

## 3. A codemod to typed accessors (`claude/bindfix`, not merged)

Converted 86 of 118 in-scope sites in two directories, byte-identical, with no model in the
loop; in scope is about half of the bound sites (the rest are zero, one, angle and loop
locals). Tree-wide estimate: 700 to 950 sites. Some output reads well, some does not (casts
to reach a parent struct, accessors on file-local struct copies). Useful as a step inside a
rewrite, not as a campaign.

## 4. Rewriting the whole function (the pilot)

Twelve functions the reviewer called contrived, picked at random among those under 110
lines; two Opus agents and one Sonnet agent, four each; instruction: rewrite from what the
function does (the ROM listing), keep the exact bytes, at most about 40 scope compiles.

| Outcome | Functions |
|---|---|
| natural match | 8 |
| partial: more natural, one or two constructs had to stay | 3 |
| no real progress | 1 |

* Cost: about 460K subagent tokens for twelve, near 40K a function; the Sonnet batch cost the
  same tokens as an Opus batch and got 2 natural and 2 partial.
* Five of the eight natural matches took one to four compiles. The steering was never
  needed: the compiler reuses a register known to be zero, hoists a loop-invariant load and
  shares a field address by itself. Earlier workers imitated the output instead of writing
  the source.
* What stayed in the partials is real: a pointer local assigned in both branches (the ROM
  computes the addresses in one branch), a scratch pointer reused for two things (an
  equivalence note halves its allocation priority otherwise), a `found` flag the ROM keeps in
  a register. These are candidates for "the original was written this way", to be argued
  with `concession-ok`.
* Siblings: seven neighbors of one pilot function carry the same pattern.
* Eleven of the twelve are landed (PR for branch `claude/naturalize-pilot`). On their files the
  linter's warnings went from 469 to 471: the natural versions call typed accessors with a
  constant, which the linter still charges. The metric moved the wrong way on better code.

## What changes

1. **The objective is the reviewer's verdict per function**, not the warning count. Judge the
   whole tree once (about 11M Sonnet tokens at the sample's rate), keep verdict, tags and
   evidence per function in a table, re-judge a function when it changes. The linter stays as
   the gate for errors and stops charging typed accessors.
2. **The unit of work is the function, rewritten from the ROM**, not a flagged site edited in
   place. Queue: contrived first, in ROM order within an object so siblings fall together.
3. **Cheap model first.** Sonnet takes each function with a compile cap; what it leaves
   partial goes to Opus with its notes; what Opus leaves is recorded as a candidate original
   construct with the compiler decision named.
4. **Tools the pilots asked for**, in order of expected yield: (a) strip-and-check: remove
   each unnatural construct in turn (parked constant, single-field pointer, known-zero store,
   bare block) and all together, keep what matches, before any model sees the function;
   (b) spelling enumeration through `scope.py try` (operand order of a sum, ternary against
   if/else stores, inline parameter order and type); (c) look up the existing typed accessor
   for a field and operation; (d) a note in `scope.py vs` when a pseudo's live length was
   doubled by a constant equivalence.
5. **No more warning lanes.** They cleared about 30 uses of debt per Codex quota point and
   left the function's shape alone.

## Not known yet

* The sample is 150 functions and one reviewer model; the 3,000 figure could be off by
  several hundred, and functions over 110 lines were not in the pilot.
* Whether the reviewer agrees with itself on a second pass, and with a second model.
* How many of the 2,700 "minor" functions are fixed by types alone (struct tools) and how
  many need naming work.
