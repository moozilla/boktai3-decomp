# Completing the largest unmatched function in ordinary C

Tags: #matching #compiler #tooling #monsters

## Result

`Golem_UpdateHitReaction` (`080C9C04`, formerly `sub_080C9C04`) now matches the
entire 8,764-byte ROM interval `080C9C04–080CBE40`. Its C definition is active in
the normal build; its `NONMATCHING` guard and assembly fallback are removed.
The expanded function and its reachable inline helpers contain no register
pins, assembly barriers, or inline assembly. No diagnostic compiler setting,
synthetic input, padding computation, or binary rewrite is required.

The complete combined build printed `build/boktai3.gba: OK`, and direct
comparison confirms that all 16,777,216 rebuilt ROM bytes equal the supplied
reference. The RAM-literal check also passed. The earlier 660-byte
`Vorn_TryRestorePatrolRoute` (`080AEB74`) remains exact.

This match contributes one function and **0.363585 percentage points** of exact
code progress. Current aggregate totals are in [PROGRESS.md](../PROGRESS.md).

| Check | Result |
|---|---|
| Complete target interval | 8,764 / 8,764 bytes equal; zero differences |
| Function SHA-256 | `e84861a56dd140dc051d61b2438dec3e0b254bcfd6a89e98277eb03514b5f4df` |
| Full ROM SHA-1 | `2651c5e6875ac60abff734510d152166d211c87c` |
| Ordinary development compiler | Exact, without diagnostic overrides |
| Independent stock compiler | Exact |
| Complete translation unit under stock build flags | Exact target; seven neighbors preserved |
| Directed execution comparisons | 34 outer-state and 48 arrival cases pass |
| Target-compiler layout assertions | 135 pass |
| Normal source integration | Transactional landing and formatted unit check pass |
| Combined ROM and RAM-literal build | Pass |
| Complete unittest discovery after review fixes | 196 tests pass |

The function is in [src/golem_2.c](../src/golem_2.c). Its provisional descriptive
name is recorded in `symbols/functions.csv`; the two callback references in
`src/fn/sub_080CFC14.c` were renamed with scope. Other functions retain their
existing implementation and concession status. The translation unit's existing
concession marker remains necessary for a neighboring function.

## What changed in response to the review

The earlier rough draft was treated as a behavioral starting point. The
reconstruction recovered actual actor, body, group, tracking, route, terrain,
saved-control, and selected-player records from the observed accesses and
matched relatives. Source changes were evaluated as hypotheses about ownership,
value domains, and control flow. Complete source variants were retained even
when the immediate distance increased.

The review's concern about local minima was borne out by the final path. A
candidate with 69 differing stack bytes was useful, but the successful branch
went through a candidate with 152 differing bytes. That source had the helper
structure needed to satisfy the exact ordering constraints. Following only the
smaller mismatch count would have discarded it.

Several earlier uncertainties are now resolved in ordinary source:

* **Random-table lifetime.** The arrival operation captures the real table
  pointer before its existing guard and passes it to direction selection. The
  chooser owns the table view; sampling evaluates its real inputs once. This
  preserves the constant-equivalence distinction that makes the ordinary
  compiler spill the shared table at the target stack slot. The earlier private
  compiler intervention is historical evidence, no longer an open requirement.
* **Callback and shared tails.** Invalid-target and late route-failure paths
  select the actual fallback callback slot and join at one invocation. Combined
  with the recovered lifetimes, this reproduces the target's shared tails.
* **Map ownership.** Cell and width methods receive the logical map owner. A
  getter for the already-loaded grid had changed load order; the logical map
  interface retains the observed pointer/offset/load sequence. Stored and
  promoted numeric domains were established through real queries and
  conversions.
* **State entry.** Selecting state, phase, and behavior is distinct from resetting
  the entry counter and latch. Installing the control binding is distinct from
  resetting motion control. The shared operations preserve every original
  store, value, and ordering relationship.
* **Geometry and saved properties.** X/Z cell-center macros capture the actual
  grid coordinate once. Terrain level conversion retains its narrow return
  boundary. Saved control reads the existing control pointer, kind, state,
  phase, and behavior in their original order through meaningful query methods.

The earlier selected-player and charge-field behavioral corrections remain in
the final source. The selected co-op player supplies its own flags, and charge
state updates use the observed charge-flags field. Typed selected-player records
replace the final literal-offset reads without changing a single target byte.

## Why the last stack bytes were difficult

After recovering instruction shape, physical register choices, branch targets,
literals, the shared RNG spill, and the stack frame, the residual was a
permutation of 61 private field-address stack slots. The target and candidate
instructions had identical offsets. A consistent bijection between their
stack slots made the remaining problem explicit.

The compiler's partial-redundancy-elimination pass orders these address
expressions by hash bucket, then by first discovery within each bucket. Reload
allocates their stack slots in the resulting pseudo-register order. That order
depends on the source's genuine inline expansion, including type conversions
that disappear before the final machine code.

For the recovered address-expression family, the measured hash is:

```text
bucket = (13772 + input_pseudo + field_offset) % T
T = (N // 2) | 1
```

`N` is the count of real instruction, jump, and call entries after address-of
elimination, immediately before GCSE. Counting an earlier dump misses real
instructions introduced by that pass. Collision order was checked against the
compiler's append-to-chain implementation and the same-byte diagnostic trace;
an earlier reverse-order assumption was rejected.

This model was verified against the candidate's actual slot assignment before
using it to infer the target constraints. It guided the choice among complete,
meaningful source APIs. It never set the compiler table size or inserted
computations solely to alter it.

| Actual source family | Differing bytes | Real pre-GCSE instructions | PRE table size |
|---|---:|---:|---:|
| Public interfaces and promoted row query | 69 | 3,450 | 1,725 |
| Complete state/engine-operation witness | 152 | 3,459 | 1,729 |
| Stored-domain saved-property queries | 11 | 3,475 | 1,737 |
| X/Z coordinate captures | 4 | 3,475 | 1,737 |
| Generic current-control query | **0** | **3,475** | **1,737** |

The 152-byte witness satisfied every relative target ordering inequality. Its
remaining failure was one absolute wrap: a target-ordered expression reached
unwrapped bucket 1761 in a table of size 1729. The later complete query and
conversion interfaces resolved that particular cause. In the exact witness,
the last expression lies at bucket 1697 within the 1737-bucket table.

The exact discovery source has these internal identities:

| Inlined copy | Arrival actor | Turn actor | Group |
|---:|---:|---:|---:|
| 1 | 121 | 812 | 816 |
| 2 | 2,155 | 2,846 | 2,850 |
| 3 | 3,449 | 4,140 | 4,144 |
| 4 | 4,778 | 5,469 | 5,473 |

All 61 target slot positions, including collision ties, agree. These numbers
describe measured compiler output; none appear as source-level controls.

Two composition errors were caught before acceptance. A captured motion-flags
macro replaces the full-value query body, so the isolated deltas cannot simply
be added. A phase-entry helper's earlier effect is absorbed after state
selection and entry reset are split. Both combinations were recompiled on their
actual source base. The final witness was materialized and measured before the
last ordering prediction was accepted.

## Verification and readability

Independent validation compiled the raw witness, the descriptive cleanup, and
the final typed-player source under the uninstrumented stock compiler and the
complete translation-unit pipeline. All produce the same exact target hash.
The seven neighboring functions preserve their source bodies and relocated
machine bytes against a fresh compilation of the baseline unit.

The directed fixtures cover outer early returns and state entries at two signed
heights, all four arrival copies, valid and invalid selected-player slots,
navigation outcomes, slopes, callbacks, and random-direction exhaustion. They
compare returns, nonstack writes, read sets, call arguments, and full navigation
coordinate inputs under frozen address-resolved callee contracts. Their finite
coverage and stub assumptions remain explicit. Exact ROM identity is the
acceptance criterion.

The target compiler also checked 135 layout assertions. These cover the
actor/group/body records, map cells and overrides, saved records, and the final
selected-player view. In particular, the compiler gives the terrain cell and
override records four-byte alignment; a host-style two-byte assumption was
rejected without changing candidate source.
These records describe the observed portions of runtime objects. The grid's
`cells[1]` member represents trailing runtime storage, without implying a
one-cell map capacity.

Cleanup preserved exact bytes at every accepted step: remove experimental
prefixes, remove unused legacy helpers and raw-offset macros, consolidate the
duplicate saved-control record, express terrain shape as an ordinary switch,
and replace selected-player offset arithmetic with verified records. Scope then
renamed the function, and the transactional landing path removed its draft
guard, checked that C is active in the normal build, formatted the unit, and
ran its byte check. The final complete ROM was rebuilt once after integration.

The helper names and tentative field interpretations are reconstructed. Byte
identity does not determine a unique historical source spelling.

## Tooling and reproducibility

The review also led to concrete matching-tool corrections:

* Context identity includes generated assembly existence/content and the ROM
  data ranges resolved by scope, preventing stale exact verdicts.
* Concession detection examines the expanded target and reachable helpers,
  including hints hidden in macros.
* Native semantic execution runs in a bounded subprocess. Crashes, signals,
  timeouts, and malformed responses become unavailable evidence while the
  parent search remains alive. Frozen contracts are not silently relaxed.
* Tests are discoverable through ordinary unittest discovery, and preprocessing
  dependency collection handles the supported Apple preprocessor input form.
* Worker commands now expose proposal, bounded readable diff, verbatim source
  export, and transactional local landing. Diff annotations require the same
  bytes and decoded rows as production. Landing independently rechecks stock
  bytes and rolls back failed formatting or unit checks.
* The statement-order family now supports a bounded useful case: constant
  stores to distinct integer fields of the same resolved struct object. It
  still rejects aliases it cannot establish, unions, bit fields, observable
  qualifiers, calls, and memory-reading right-hand sides. Qualifier analysis
  follows typedefs, closing a concrete hidden-volatile reorder defect.

The focused move suite passes 24 tests. The reviewed contact-slot initializer
now exposes the three safe adjacent swaps of its four constant field stores.
Independent host-C compilation checks the resulting member values. Complete
unittest discovery passes all 196 tests after the review fixes.

The delivered archive includes the final source overlay, patches, causal
reports, validation summaries, replay scripts, and portable source histories.
It also preserves rejected source controls and the earlier checkpoints, clearly
marked as historical. Historical evaluations require recompilation in the
destination context; they are not imported as authoritative new verdicts. ROMs,
generated assembly, executable blobs, and live databases are excluded.

Principal evidence paths within the retained experiment tree are:

* `round3-structure/EXACT_MATCH_CAUSAL_REPORT.{md,json}` and
  `phase_saved_control_query-constraints.json`;
* `round3-callback/exact-validation/`, including the final whole-unit stock
  result and the 34/48 fixture reports;
* `round3-value/layout-verification/exact_player_layout.json`;
* `round3-macros/exact-cleanup.json`, `exact-player-view-result.json`, and the
  transactional landing evidence;
* `round3-final-build.log` and the final integration summary.

This closes the largest-function target from the
[earlier scale report](matching-search-scale_2026-10-29.md). The useful reusable
lesson is to retain distinct source hypotheses, diagnose the compiler decision
behind a mismatch, and validate their actual composition. The largest function
now supplies an end-to-end example at scale; other compiler mechanisms still
require their own diagnosis.
