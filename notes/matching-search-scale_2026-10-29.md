# Matching search at the largest unmatched function

Tags: #matching #compiler #tooling #monsters

> Historical checkpoint: the remaining target described below is now an exact
> ordinary-C match. See the [completion report](matching-search-completion_2026-10-29.md)
> for the final source, compiler diagnosis, and full-ROM validation.

## Outcome and scope

The earlier `Vorn_TryRestorePatrolRoute` (`080AEB74`) result is a complete
660-byte ordinary-C match. It was independently checked with stock agbcc and
the full ROM build. That result is unchanged.

At the owner's request, this follow-up selected the largest unmatched function
in the supplied snapshot: `sub_080C9C04`, an 8,764-byte golem reaction dispatcher
in `src/golem_2.c`. The checked-in progress inventory contained 359 unmatched
functions; the next largest was the 6,372-byte `Golem_ReactToContact`
(`080C12FC`). Size is an objective stress case, not a proof that this function
is the hardest compiler problem in the project.

**The largest function remains unmatched.** Its reconstructed ordinary C has
no new register pins, barriers, or inline instruction bodies. It remains behind
`NONMATCHING`, and its normal build uses the existing assembly placeholder.
This follow-up contributes **0.000 percentage points of exact code progress**.
The existing concession marker on the translation unit belongs to other,
unchanged functions.

The work materially improved the source, repaired two real behavioral errors,
and identified specific remaining compiler decisions. It also exposed and fixed
analysis, execution-model, and persistence defects that a small example did not
exercise. It does not establish an automatic general-purpose matching solver.

## Measurements

The target contains 3,893 decoded instructions, 558 resolved CFG blocks, 63
external calls, and 12 internal Thumb `BL` long jumps. Four large inlined copies
of the arrival/route logic dominate its size. Internal long jumps must not be
treated as external ABI calls.

| Measure | Original draft | Reviewed ordinary-C draft |
|---|---:|---:|
| Target bytes | 8,764 | 8,764 |
| Compiled bytes | 8,896 | 8,620 |
| Call-anchored unmatched instructions | 2,033 | 333 |
| Changed scalar operands in that alignment | 207 | 168 |
| Sum of those two structural measures | 2,240 | 501 |
| Scope instruction difference | 6,901 | 1,193 |
| Scope shape distance | 2,626 | 717 |
| Full-unit draft fuzzy similarity, same current toolchain | 13.1% | 75.9% |
| Exact match | no | no |
| New pins / barriers / instruction bodies | 0 / 0 / 0 | 0 / 0 / 0 |

Both structural columns use the same bounded minimum-edit algorithm within
ordered external-call intervals. The approximately 77.6% reduction is a
reduction in that **heuristic distance**, not a matched-code percentage. Earlier
greedy-aligner numbers are not comparable. All 63 external-call anchors agree
in sequence, but repeated callees and tied instruction alignments are still
ambiguous.

The reviewed candidate's machine-code SHA-256 is
`8686b08b94c835e2d46187de3cbfed2f01ce4efbd4069c541fd44c5fb45c6965`.
Stock and instrumented compilers independently produce it. Its stack allocation
is `0x14C`; the target's is `0x150`. A direct positional byte comparison still
finds 8,389 differing or missing bytes. Relocated branches, literal pools, and
layout changes make that positional measure poor guidance for a large function,
but the exact acceptance condition remains zero differences.

The actual stock-compiled full translation unit, its canonically relocated
object function, and the search evaluator all produce that same candidate hash.
All seven other emitted functions retain identical canonical compiled bytes.
The original source's 12.6% target and 77.7% neighboring-draft annotations were
stale: a controlled rebuild measures 13.1% for the original target and 80.8%
for the neighboring draft both before and after this change. Recording the
current scores changes that neighbor's annotation, not its source behavior.

## What the source reconstruction recovered

### Two semantic repairs

The arrival logic selected a co-op player for its terrain-cell calculation but
then tested player zero's flags. The ROM carries the selected player index
through both accesses. The reconstructed helper now does the same, including
the invalid-slot and non-co-op paths.

In the nested charge branch, the draft set bit `0x80000` at actor offset `0x2CC`
and then cleared bit `0x80` at that same field. The ROM sets the former bit at
`0x2DC`, then clears the latter at `0x2CC`. Eight targeted charge fixtures
disagreed before this repair and agreed afterward. The repair slightly worsened
the then-current structural score, from 632 to 634; the ROM evidence took
precedence over that score.

This matters to the search design: transformations that preserve an incorrect
starting draft's behavior cannot reach the ROM. Free-form model proposals must
be able to repair behavior, while guarded AST moves remain useful for narrower
equivalence hypotheses.

### Types, helper boundaries, and original-object lifetime

The useful edits were connected to compiler evidence rather than a sweep of
surface spellings:

* Boolean helpers returning `u8` and flag helpers returning masked `u16` values
  recover distinct expansion and lifetime behavior.
* Arrival success and the caller's second, partner-position route attempt are
  separate source boundaries. Keeping their cleanup and return paths separate
  avoids duplicated address setup.
* Signed height values pass through an `s32` helper and restore the target's
  signed halfword loads. Full-width negative masks are retained until the final
  halfword store.
* The original incoming object and a typed local actor alias have different
  compiler lifetimes. Keeping the original object for selected continuations
  naturally restores `sl` and all four saved group-pointer stack slots. The
  inner arrival callback instead uses its local actor alias; changing outer
  callbacks the same way was a measured regression.
* A chained assignment of the aligned position value loads the pair once and
  writes both destinations.
* A typed direction-field store produces the target's early zero definition;
  the raw byte-array store delayed it until a later packet-kind store.

The random-direction chooser is especially instructive. Separate early-success
and exhausted-retry returns preserve a real fallback copy. A unified result
variable let CSE2 use the failed comparison's equality fact to erase that copy.
Keeping the two returns increases the prior direction's reference count and
allocation priority, naturally placing it in `r5`, with the retry counter in
`r7`, as in the target. A simpler scoped `for` form compiles identically to the
corresponding `do`/`while` form and is retained in the reviewed source.

The cleaned source removes unused reconstruction helpers and experiment names.
Every cleanup stage preserved the selected machine-code hash. Other function
definitions in `golem_2.c` are textually unchanged.

## Execution evidence and its limits

The reviewed source passes 82 directed ROM/candidate trials: 34 outer-state
cases and 48 arrival cases, visiting 395 distinct target blocks in combination.
The same 82 cases also pass when fed the actual full-unit object bytes directly.
The cases cover signed heights, charge states, co-op selection and player-one
flags, invalid player slots, both slope axes, early RNG success and four-attempt
exhaustion, motion-flag exclusions, route outcomes, and partner-route fallback.

The fixtures construct valid actor, context, player, terrain, route, and RNG
memory. They execute the arithmetic for `Div` and `Mod`, supply directed
navigation/terrain results, and use deterministic ABI stubs for other callees.
They compare return values, nonstack writes, read sets, calls, and all three
coordinates of route-vector arguments. This is more useful for this function
than arbitrary random pointer memory, but it is still a finite, conditional
test model. It is not a whole-function equivalence proof or a game-playthrough
validation.

One apparent failure was a checker defect: the ELF chose a `__addr` alias for
a callee, while its arity map used only the source spelling. Unused argument
registers were then compared and fed into synthetic return values. The core
checker now binds frozen baseline contracts by ROM address and maps them to
the selected ELF labels, including indirect-call labels. Candidate declarations
cannot change the oracle's call arguments, input sampling, or return observation.

The conservative fix also exposed two prototypes missing from the original
translation unit. Directed fixtures explicitly supplement them from the
existing `scene_actor_placement.h` and `scene_actor_animation.h` headers, with
header hashes recorded as model assumptions. The initial unknown-arity runs
are retained and labelled; unknown contracts in the general tool still observe
all four core argument registers and possible outgoing stack arguments. A
four-register contract never silently discards a fifth stack argument.

## The remaining compiler decisions

### A constant-equivalence note explains the random-table stack discrepancy

The target keeps the random-table pointer at `[sp+0x14C]`, uses `r8` for the
wrap mask and `r1` for the output mask, and allocates `0x150` bytes of stack.
The ordinary draft keeps the table in `r8`, with the masks in `r1` and `r3`,
and allocates `0x14C` bytes.

The register-preference chain is now explicit. A later load of the control
pointer into locally allocated `r1` gives the hoisted field address an `r1`
preference. That address eventually
spills, but its preference still makes the output-mask pseudo avoid `r1` on the
first global-allocation pass. The table has lower priority because
`local-alloc.c:update_equiv_regs` doubles the live length of a register carrying
a constant `REG_EQUIV` note. Reload can later rematerialize that constant instead
of giving it a real stack slot.

A priority-only diagnostic restored the desired initial ordering but merely
reloaded the literal inside the loop. It did not reproduce the target's stack
slot. A second, isolated diagnostic suppressed the `REG_EQUIV` treatment only
for `gRandomTable`. With no knob, that private compiler reproduced the
production hash. With the knob, it produced the exact `0x150` frame and all
four table stores to the shared `[sp+0x14C]` slot. It also recovers all the
examined roles together: wrap in `r8`, output mask in `r1`, prior direction in
`r5`, retries in `r7`, and seed pointer in `r2`. The first 23 RNG instructions
then agree after resolving literals and translating local branch locations;
this is normalized agreement, not a raw-byte match. On the pre-cleanup DP509
candidate, this changed 8,620 bytes / structural distance 509 to 8,652 bytes /
distance 483. The result remained unmatched.

That experiment proves the causal compiler intervention for the frame and
table slot. It does not prove the original source form. The private compiler
patch, its outputs, and its measurements are diagnostic evidence only; none
can count as an accepted C match. The shared compiler and normal build flags
were not changed.

Ordinary source controls include explicit RNG inputs, mutable helper inputs,
separate fallback returns, working-pointer table access, literal-address
spelling, scoped-loop forms, and a typed result store. Equal repeated constant
definitions still retain equivalence: the compiler explicitly permits several
sets with the same invariant note. The unresolved source problem is to recover
the value identity or lifetime that avoids this equivalence naturally, without
inserting a barrier, pin, or artificial instruction.

Five final controls tested a mutable local table, a flattened chooser body,
helper input/result reuse, a pointer used successively for the real seed update
and table lookup, and a union carrying those active values. None removed the
equivalence while preserving the required hoist. The shared forms instead
create one table pseudo spanning all four copies, with four equal invariant
definitions; repeated live-length adjustments lower its priority further and
reload rematerializes the table inside the loop. All proposals remain distinct
history nodes, including the local-table and flat-body forms that compile alike.

### Tail sharing and physical-register constant reuse

The largest remaining outer interval includes different tail sharing after
allocation. The ROM emits a local Settle prefix and shares its final state
suffix. The draft's two physical Settle sequences become identical, so the
compiler's post-allocation cross-jump pass shares the entire sequence instead.
The first prefix survives through the `greg` dump and disappears in `jump2`.

Changing one first-copy reload choice with a diagnostic rotation makes the
first three address instructions reappear locally and moves the shared entry
to the callback load. This connects the control-flow difference to physical
register choices; it does not justify inserting a speculative source branch.
The target's first and final copies use the same actor register but different
temporary offset registers.

Related differences are alternative recipes for the same field addresses:
loading an offset directly versus adding to a physical register that already
holds a nearby constant. `reload_cse_move2add` tracks those physical contents
and invalidates them at labels. A target literal load does not uniquely imply
an extra source label: different preceding register allocation or contents can
also explain it. The preserved audit connects ten such examples to exact
instructions and compiler dumps. The next source experiment should revisit
these decisions after the random-table lifetime is resolved, because a changed
spill set may change both automatically.

## What changed in the reusable tool

### Analysis that remains explicit about uncertainty

`stackflow.py` adds conservative stack-value transport to the raw physical
dependency graph. It tracks widths, overlapping stores, SP changes, push/pop,
joins, and loops. Identity aliases carry reaching stores and assumptions;
truncating/extending accesses retain typed-bit facts when identity is not known.
Bounded value and iteration sets report exhaustion instead of inventing a
dependency.

Strict mode invalidates on potentially aliasing writes and calls. On this
target it forwards **zero** loads. The optional `abi-private` mode states its
private-frame, non-escape, no-forged-pointer, and asynchronous-write assumptions
and forwards 107 target loads in the measured run. It is useful conditional
evidence, not a reason to silently weaken the default. Resolved-CFG limitations
and unknown indirect successors remain visible in reports.

`boundaries.py` aligns external-call sequences before instructions and uses
bounded dynamic programming inside each interval. It avoids a greedy aligner's
tendency to pair different repeated store sequences. It reports incoming,
outgoing, and local value constraints, including upstream producers and stack
alias provenance. Local byte agreement never proves a compatible entry binding.
Instruction alignment and region status remain hypotheses with `proof: false`.

The AST catalog can edit an explicitly selected inline helper while continuing
to evaluate the original complete machine-code function. A 42-candidate guarded
slate over four selected definitions reached a plateau; it did not replace the
source-boundary diagnosis. All those alternatives survive in the graph.

### Persistence and reproducibility

Two independent WAL-backed search databases became malformed after reopening
in the managed workspace. A standalone WAL stress test did not reproduce the
failure, so its underlying filesystem/SQLite cause is unresolved. Both damaged
databases were preserved. Source files, result sidecars, and a checked snapshot
allowed recovery into new stores; replayed candidates reproduced their hashes.

The store now defaults to DELETE rollback journaling with full synchronization,
checks integrity before mutations, and offers an explicit checked backup API.
Snapshots use SQLite's backup operation and atomic no-overwrite publication,
not a blind copy of the main file. Recovered stores and snapshots passed
separate-process reopen, database, and foreign-key checks. Portable histories
add a second representation of sources and hypotheses independent of binary
caches. This mitigates the observed failure mode without claiming its root cause
is solved.

The final regression run passes 168 tests and five subtests, covering stack
widths and overlap, calls and escapes, internal long jumps, local-pointer and
internal-operand correspondence, frozen call aliases and fifth arguments,
helper edits, persistence, concurrent claims, and existing build behavior.
The normal guarded unit matches its entire 12,492-byte ROM extent; Vorn's
independent unit check remains exact. The combined build prints
`build/boktai3.gba: OK` and passes the RAM-literal check. Because the golem draft
is guarded, those normal-build checks validate integration while its original
assembly still supplies the unfinished function.

Physical compiler context and analysis versions stay separate. Old analysis
must refresh after the stack/boundary fixes; old source histories never inherit
an exact verdict in a different compiler context. Diagnostic compiler changes
remain outside ordinary candidate acceptance.

## Evidence and continuation

The updated handoff package contains the combined patch, per-commit series,
changed-file overlay, accepted Vorn source, reviewed guarded golem source,
portable source histories, directed-fixture scripts and results, and the
isolated compiler diagnostic patch/report. It excludes the ROM, generated
assembly, compiler binaries, and live SQLite caches.

The main experimental records in the working checkout are:

* `build/matchsearch/largest-final/validation.json`: source/hash, independent
  stock check, structural metrics, and the directed fixture model/results.
* `build/matchsearch/largest-final/selection.json`: largest-function selection.
* `build/matchsearch/largest-final/full-tu-validation.json`: full-unit/search
  byte identity, unchanged neighbors, controlled fuzzy scores, and actual-object
  directed execution.
* `build/matchsearch/largest-helpers/RNG_BOUNDARY_FINDINGS.md`: successful and
  rejected chooser/typing/lifetime hypotheses.
* `build/matchsearch/largest-worker-recovered/CALL16_AUDIT.md`: post-allocation
  tail sharing, constant reuse, and the bounded reload counterfactual.
* `build/matchsearch/noequiv-compiler/`: private constant-equivalence
  intervention and no-knob reproduction check.

Continue on `080C9C04`: recover the table's ordinary-C value identity, inspect
the resulting Settle sharing, and keep validating the two repaired behaviors.
The useful scale result is that the workbench now handles this function's
whole-function compilation, source branching, machine-value analysis, and
compiler interventions with preserved evidence. An exact large-function match
is still the missing demonstration.
