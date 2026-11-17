# Round 14: flash and interaction-marker once-loops

Tags: phase3, once-loop, background-flash, interaction-marker, register-allocation

Base: `c5883ff7d72e754337895289628075d887ba37c2`.

All five once-loops in the four assigned units were removed with ordinary C.
Each complete unit independently matches the stock ROM bytes, and each focused
linter result is now zero errors and zero warnings. No header, compiler, linter,
or diagnostic suppression changed.

| Function | Source commit | Complete stock extent | Once-loops removed |
|---|---|---:|---:|
| `BackgroundFlash_UpdateAppearing` | `6a043dcb8` | 108 bytes | 1 |
| `BackgroundFlash_UpdateDisappearing` | `1d482cca6` | 68 bytes | 2 |
| `InteractionMarker_TickIdle` | `dd97bb530` | 392 bytes | 1 |
| `SavedPositionMarker_TickIdle` | `eb5f8c692` | 336 bytes | 1 |

## Recovered operations

Both marker updates test whether scene interaction is enabled, then whether the
player has newly pressed A. The old source copied that key bit through mutable
`pressed` and `edge` locals and a do-once loop. The direct
`if (gPadState[1] & 1)` predicate removes those temporaries and matches exactly.
`StackedActor_TickIdle` in `src/fn/sub_08052A40.c` independently uses the same
scene-enable/new-A-press sequence and the same blocked-player test. The canonical
input declaration identifies the pressed-key storage as a halfword, and
`Input_UpdatePadStates` in `src/runtime_input.c` establishes the held/pressed/
released layout and the edge calculation.

The disappearing-flash update enables BG1, conditionally hides it on even frames
through frame 9, and then returns to the inactive state and hides the layer.
Forming `flags = gDispCntShadow | 0x200` directly removes the redundant previous-
flags snapshot and permits both existing do-once scopes to disappear. The
complete operation is ordinary display-state manipulation, not a relocated loop
inside a helper or macro.

The appearing-flash update has three real, mutually exclusive frame ranges:

| State frame | Hidden phases, using `frame & 3` |
|---|---|
| 0–14 | 0, 1, 2 |
| 15–29 | 0, 1 |
| 30 onward | 0 |

After frame 44 it changes state. Replacing its hand-written goto layout with
nested range conditions puts each actual hide operation in its corresponding
live branch. Along with the direct BG1 enable expression, ordinary compiler
tail merging reproduces the complete 108-byte ROM function. No extra branch,
dummy operation, or alternate weighting loop was introduced.

## Why direct deletion alone was insufficient

| Function | Plain deletion result | Subsequent source repair |
|---|---|---|
| Appearing flash | 104 versus 108 bytes; 14 realigned instruction differences | Direct enable expression, then structured live frame ranges |
| Disappearing flash | 64 versus 68 bytes; 4 realigned instruction differences | Direct enable expression |
| Interaction marker | 392 bytes; 5 realigned instruction differences | Direct pressed-key predicate |
| Saved-position marker | 336 bytes; 5 realigned instruction differences | Direct pressed-key predicate |

The initial flash and marker candidates let agbcc remove a mask-to-result copy.
Reading the input/display values as halfwords, or retaining ordinary lexical
blocks, did not by itself repair those differences. Existing
`BackgroundFlash_ShowLayer`/`BackgroundFlash_HideLayer` helpers were also tested;
using them throughout introduced a different reload or instruction sequence.
A combined scene-enable/new-A predicate was a genuine repeated operation, but
its inline-helper candidate did not match and was not adopted.

For appearing flash, the direct enable expression first restored the complete
instruction shape, leaving twelve register-dependent bytes. The canonical
diagnostic compiler gave `flags` priority 5,384 and `frame` priority 6,086;
`AGBCC_PRI=flags:+703` matched as a diagnostic experiment. The adopted source
uses the actual frame-range structure, so its live branch references provide
the allocation behavior without a compiler knob. The knob is evidence about
the mismatch, not part of the accepted build.

## Remaining named masks: source choices, not established concessions

The existing `visible` locals in the flash functions and `button`/`blocked`
locals in the marker functions remain. Substituting literals changes code:
each marker's individual mask substitution changes two instructions; removing
the appearing-flash mask from its accepted range structure produces 116 rather
than 108 bytes. The disappearing-flash direct-mask candidates also differ.

Those measurements establish code-generation dependence only. The names carry
real display, input-enable, and player-status meanings; this investigation did
not establish that the locals were introduced solely to manipulate allocation.
They are unresolved source choices, and are not counted as proven artificial
scaffolding. Earlier worker commit messages used “allocation holdout” for the
measured dependence; that wording must not be read as a provenance claim.
The focused `const-local` detector only recognizes locals named `zero` or `one`,
so its clean result does not adjudicate these named masks. All failed literal
substitutions remain reviewable in the evidence.

## Durable evidence and verification

[round14-flash-marker-candidates_2026-11-16.jsonl](evidence/round14-flash-marker-candidates_2026-11-16.jsonl)
contains 60 complete source states as independently applicable patches against
the base commit above, including the four accepted states. It records the
hypotheses, source hashes, historical function scores, accepted commits, and
independent full-unit stock extents. Every patch was reconstructed and its
source hash verified. Generated assembly and RTL are not included.

Complete candidate files, stock logs, focused linter comparisons, and portable
graph histories remain under `build/round14/flash-marker/`. The live graphs are
`build/matchsearch/round14-flash-marker-*`. All candidates compiled; the four
accepted sources were separately checked over their complete original extents.
No full build or whole-tree scan was run by this worker; combined integration
and ROM verification remain with the coordinator.
