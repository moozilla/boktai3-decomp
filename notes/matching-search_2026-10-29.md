# Matching search: implementation, evidence, and next steps

Tags: #matching #compiler #tooling #process

This is the third implementation of the supplied matching-search brief, based
on repository commit `9832a33eb6d8c6df0c4bc6990184207a3dffb908`. The entry point is
[`tools/matchsearch/README.md`](../tools/matchsearch/README.md). The implementation
is a persistent Python workbench in which the calling model chooses experiments.
It has been used on four of the supplied functions. One new function is an exact
ordinary-C match; the other results below are explicitly incomplete.

## Result and acceptance

`Vorn_TryRestorePatrolRoute` (`sub_080AEB74`, 660 bytes) now matches with ordinary
agbcc, without register pins, asm barriers, handwritten instructions, or compiler
knob overrides. The independent per-unit checker reported zero differing bytes.
The normal combined build also reported `build/boktai3.gba: OK` and passed the
RAM-literal check. The accepted change is isolated in commit `21f3028`.

| Target | Ordinary-source outcome | Status |
|---|---|---|
| Vorn, `080AEB74` | 26 aligned instruction differences to zero; 660 bytes | Exact; landed locally |
| Player, `08141BAC` | Pin-free 108-byte candidate, nine register-operand differences, same register-blind instruction shape | Unmatched; retained source |
| Damage-number renderer, `08046BC8` | 68 to 11 aligned instruction differences; 552 bytes versus a 548-byte target | Unmatched; retained two complementary branches |
| Boss state machine, `081AA788` | 670 to 441 aligned instruction differences; 1,652 bytes versus 1,672 | Unmatched; source-repair candidate |

The boss comparison is against the source actually supplied in the package.
A historical worker reported a shape-distance-172 candidate whose source was not
provided. The retained shape-distance-254 result does not surpass that report.
Player's supplied pinned draft had fewer register differences than the new
pin-free branch; the new branch removes its hard-register assumptions and
isolates the remaining decisions. Neither outcome counts as matching progress.

The four original sessions retain 209 source states in total. Four exact source
spellings in the Vorn session are one function match, not four new matches.
Their original database paths are `build/matchsearch/vorn`, `player-worker`,
`damage-worker-2`, and `boss-worker`. Portable JSON histories preserve their
sources and hypotheses across toolchain changes; generated databases, binaries,
ROM data, and compiler dumps remain outside git.

## Architectural judgment

### Recover machine constraints first, with provenance

The ROM determines instruction semantics, access widths, machine sign-extension
and condition behavior, constants, resolved control edges, direct call
destinations, and observable memory operations. It does not uniquely determine
original C signedness or resolve every indirect edge. `constraints.py`
lifts these into definitions and uses, reaching definitions through loops and
joins, separate NZCV dependencies, and a control-flow graph. Copies preserve
value provenance. An affine address projection recovers simple field accesses
while keeping loaded pointers and unresolved merges opaque.

This representation is useful before a source variable has a name. For the
boss example, it recovers signed halfword accesses at actor offsets 0x54, 0x56,
and 0x58, and the actual seven-way dispatch table. It preserves case order and
does not claim to prove the index bounds. Unsupported indirect edges remain
explicitly unknown.

There are three evidence levels:

1. **Direct machine facts:** decoded operations, byte comparisons, access width,
   direct branch destinations, and compiler-produced bytes under a frozen build.
2. **Stated assumptions:** baseline function declarations and the 32-bit ARM ABI,
   emulator input domains, and stubbed-callee behavior.
3. **Heuristic correspondence:** alignment between changed instruction streams,
   source names mapped onto compiler pseudos, and dependency-compatible regions.

The implementation keeps these levels distinct. Candidate prototypes cannot
change target call assumptions: `contracts.py` derives supported explicit
prototypes from the frozen baseline and applies the same map to both streams.
Unknown, conflicting, variadic, aggregate, multiword, or attributed signatures
retain conservative argument dependencies. On the boss baseline, the inferred
map covers 14 of 15 distinct direct call targets; the compiler division helper
has no prototype and stays unknown. A source declaration is still an ABI
assumption, not a proof about the ROM callee.

### Inverting allocation cannot uniquely recover original C

The supplied agbcc sources support a narrower and more useful inverse problem.
`global.c:allocno_compare` orders candidates using references, live length,
size, and allocation-number tie-breaking; `find_reg` then considers conflicts
and register suitability. `local-alloc.c:combine_regs` joins quantities, and
the final assignments can map several pseudos through one quantity.
`reload1.c` describes per-instruction temporary reload registers, spilling,
reassignment, and inheritance. Consequently, observing one hard register does
not identify one original source variable, and `reg_renumber` is not invertible
into a unique source partition.

The workbench therefore uses inverse constraints to generate forward tests.
Its `decision_delta` compares uniquely named values' reference counts,
lifetimes, priorities, and assignments; it refuses to pair unnamed pseudos just
because their numbers happen to agree. Reload-set and rotation changes are
separate evidence. A diagnostic force can establish which allocator choice
matters, but does not establish that the proposed pseudo partition or forced
allocation was valid in the original program.

This work also corrected an actual scope instrumentation ambiguity. A reload
trace with `start=-1` could mean either the first rotating choice or a forced
group scan. The trace now records `rotate`, and both prediction and explanation
handle these cases separately. A live Player intervention changed the first
reload from r0 to r3 as predicted. Legacy ambiguous traces remain unknown.

### Preserve source states, share expensive binary work

Each source state is content-addressed under an immutable physical compiler
context. SQLite stores sources, edges, hypotheses, predictions, evaluations,
evidence, retention decisions, and generation-fenced worker leases. The source
graph permits multiple parents and cycles from inverse moves.

Equal binaries share their graph analysis, but their source nodes remain
distinct. A helper boundary, temporary, or lexical scope eliminated in this
compile can change the next transformation's effect. Eighteen of twenty
targeted Damage catalog variants produced their parent's binary; retaining
their different source neighborhoods is deliberate.

Physical context and analysis versions are separate. Changing the ROM,
toolchain, flags, headers, symbols, or relevant compiler tools invalidates the
physical context. Changing the analyzer marks its metrics stale and excludes
them from frontier ranking until refreshed, usually from cached bytes. A
`fork` imports source history into a new context without importing old scores
as current verdicts, or old rejections and protections as active decisions.
Interrupted initialization can resume with the same configuration.

### Use a bounded action set and explicit source repairs

The catalog has 17 default guarded transformation families and a default cap
of 256 candidates per parent. It round-robins families and limits generated
locals. These are bounded action sets, not a completeness theorem about all C
spellings. Stable move IDs include the parent, site, and result; AST paths and
preconditions explain the edit. Legacy speculative rewrites require explicit
selection and are labelled hypotheses.

Guards account for lexical binding, volatile accesses, effects, narrowing,
initializer dependencies, writes, loop scope, `continue`, and return/goto
fallthrough. Guarded equivalence is relative to defined parent-C behavior.
It does not prove agreement with the ROM.

The model-proposal path also accepts semantic repairs. This is necessary:
equivalent rewrites of an incorrect draft cannot reach the target. Each
proposal records the claimed source/compiler cause; compile failures and
unproductive experiments remain in history. A prediction is a recorded
intention, not a fact certified merely because it was entered.

### Keep several objectives and require a causal explanation

The frontier exposes dependency, control, operation, instruction, size, and
concession differences separately. It selects retained hypotheses, objective
champions, structural and regional alternatives, and an exploration slot.
It never deletes dominated nodes. Source token count is a small complexity
preference, not a measure of historical authenticity.

Two measured examples show why this matters. The first useful Damage camera
change cut instruction differences from 68 to 33 while increasing unequal
bytes from 225 to 284. The boss's largest call-boundary repair reduced dependency
distance from 726 to 511 while worsening unequal bytes by 22. A byte-distance
hill climber would discard both steps.

Those boss graph distances are historical measurements from before baseline
prototype contracts were integrated. They compare candidates under the same
analyzer version; refreshing them with the final analyzer can change the
absolute distances. Instruction, shape, size, and byte counts are unaffected.

This is a diversity-based frontier with a model policy. It does not implement
trained rollouts or claim to be complete MCTS. The valuable part of the AlphaGo
analogy is the division of labor: the model chooses hypotheses; the environment
owns cheap experiments, durable branches, evidence, and an exact terminal
criterion. A tree policy can be added after there is useful empirical evidence
about which search features predict success.

### Decompose reasoning while compiling the whole function

Region reports expose live-in/live-out definitions, local byte agreement, and
dependency compatibility. Fingerprints normalize corresponding producers and
successors into target coordinates; an unrelated earlier insertion does not
automatically invalidate a region. Missing correspondence stays unresolved.
Protections report changes and uncertainty instead of silently pruning.

Every candidate is compiled as a complete function. Allocator competition,
PRE transparency, reload rotation, and literal placement cross block boundaries.
Even identical block bytes need compatible entry registers before they prove
the block's behavior in context. The current graph does not symbolically
forward stack spills or prove general memory-alias independence. These are
important limits on any stronger component-solving claim.

## Worked match: Vorn

The supplied Vorn draft already had the right 660-byte extent, but 26 aligned
instruction differences. Its reloads used r0-r3; the target needed r4 to become
available at the relevant points. The useful source changes were:

1. Preserve the motion pointer at the moment the inline flag helper tests it,
   through an output parameter. This reproduces the pointer capture before
   boolean control-flow expansion.
2. Remove an artificial narrow `k1` local and use the existing typed bit-clear
   helper with the actual constants at their uses.
3. Evaluate the restore-flag helper into `restoreBits` before constructing the
   later `flags2D0` pointer.

The third edit made a very small-looking source-order change that affected
whole-function liveness. The trace compared five uniquely named values. The
`flags` live length changed from 43 to 42 and its priority from 2325 to 2380;
other values' lifetimes changed too. Both versions had 30 reload events, but
the available spill set expanded from r0-r3 to r0-r4. That ordinary source
schedule reached exact equality without a forced allocation or rotation.

The trace establishes that these edits changed allocator/reload conditions;
the single `flags` statistic alone is not asserted to explain the whole chain.
The exact result was then cleaned: unused helpers and locals were removed,
helper names made meaningful, several established struct fields substituted,
the NONMATCHING guard removed, and the file formatted. Independent stock agbcc
still matched all 660 bytes. Sixteen emulator trials also agreed, but byte
identity and the production build are the acceptance evidence.

## Useful unresolved branches

### Damage-number display

The best ordinary branch names digit pointers in address-evaluation order and
restores camera and arithmetic value structure. It remains at eleven aligned
instruction differences. A separate `currentGroup` loop-cursor branch removes
the unwanted world-address PRE with ordinary C and recovers the target stack
frame, but worsens the instruction score to 25 while improving shape distance
from 14 to 4. Both branches are retained.

In that cursor branch, the world-address temporary's priority is 4615 and
`currentGroup`'s is 3473. Raising only `currentGroup` by 1143 crosses exactly
that threshold and yields a diagnostic 548-byte candidate with two instruction
differences. A much larger boost worsens unrelated decisions. The residual is
the final call's first-argument reload order relative to third-argument
arithmetic. No ordinary C spelling reproducing both decisions was established.
The best ordinary candidate passed eight sampled trials with 66.67% block
coverage under the existing harness model.

### Player smoothing

A signed 16-bit return local preserves sample and result as different values
through CSE; combine removes a redundant extension. This restores the target
instruction shape without pins. Nine register differences remain. A priority
intervention on `history` reduces them to three, and one reload-rotation
intervention produces diagnostic byte equality. Neither control is accepted
as a match. The pin-free source passed 132 directed trials covering all nine
reachable target blocks, including sentinel, neighbor, mismatch, and signed
extreme cases. Independent stock agbcc agrees with the fast compiler's still
unmatched candidate bytes.

### Boss state machine

The largest improvement restores which target and part pointers are captured
before `RandomIndex`, while the sprite is loaded after it. This reproduces
the target's call-boundary spills. Other changes recover a timer snapshot,
four stack objects, coordinate ordering, signed helper arithmetic, and visual
component bases. The result still has substantial structural differences;
allocator sweeps are premature. Sixty-four tailored scenarios agreed at 72.5%
block coverage with specified memory and callee stubs. The actual callees and
all valid callers were not proved. The work is retained for continuation.

## Measured cost and validation boundaries

| Measurement | Observed result |
|---|---|
| Warm Player evaluation, preprocessed source, 20 runs | Median 6.49 ms; 5.41-7.63 ms range, including decoding/alignment |
| Final Player evaluator with include validation, ordinary source, ten runs | Median 12.07 ms for precheck, evaluation, and postcheck |
| Golem, 4,304-byte target, five runs | Median 126 ms evaluation; median 43.5 ms compile/assemble |
| Final Golem value graph, three runs | Median 86.71 ms analysis plus 50.69 ms comparison, including affine projection and baseline call contracts; separate from evaluator timing |
| Twenty selected Damage AST candidates | Approximately 896 ms end to end, including session/graph work |
| Export fidelity | 65 candidates across all six supplied examples; regenerated C and preprocessed AST versions had identical text bytes and relocation triples |
| Combined ROM build | Exact identity and RAM-literal check passed after the landed Vorn change |
| Final regression run | 95 tests passed across the six new test modules and the existing build tests |
| Final integration replay | All four retained candidates independently reproduced with stock agbcc; resident stdio status/evaluate/moves/contracts/shutdown passed |

The baseline linker also exposed an independent build bottleneck. It scanned
10,175 explicit file/section selectors and was still running after more than
seven minutes. Declaring the same objects once in ordered `INPUT`, then
collecting their `.text` sections, linked the same image in approximately
2.55 seconds in the controlled comparison. `tools/build.py` now emits that
form. The ordinary build verifies the same ROM hash; elapsed times are local
measurements, not a general benchmark guarantee.

Tests target concrete soundness and persistence failures: dependency changes
hidden by register blanking, carry flags, branch and switch destinations,
truncation and shadowing, return fallthrough, diagnostic-result isolation,
context invalidation, analysis-version rollback, stale worker fencing, cyclic
source imports, and interrupted initialization. Host-compiled move tests and
the real-export checks complement each other. Synthetic tests are not counted
as game decompilation progress.

## What I would build next

1. **Stack and boundary value transport.** Symbolically forward known stack
   stores/reloads and compare component entry/exit value bindings. This should
   reduce false dependency noise before introducing stronger pruning.
2. **Compiler-decision observations by pass.** Tie a chosen source value to its
   CSE/PRE/local/global/reload history with stable semantic descriptors. The
   current named trace and counterfactuals are a practical start.
3. **Validated semantic contracts.** Recover callee effects and valid caller
   domains for an actual subsystem. Only then should a sampled counterexample
   become an automatic rejection under a stated contract.
4. **Empirical search policy.** Use the persisted experiments to measure which
   hypotheses and structural features precede real exact matches. Introduce
   tree-policy scheduling with that evidence rather than treating an arbitrary
   scalar diff as an already calibrated value function.

The useful deliverable is already runnable: a model can enumerate a bounded
slate, submit a source hypothesis, compare value constraints and named compiler
decisions, retain regressions, resume after interruption, and independently
verify an exact candidate. The next investment should improve the causal
information available for those decisions.
