# Round 14 checkpoint 4: indexed pool callbacks

Tags: phase3, checkpoint4, once-loop, pool, callbacks, feather, shared-types

Source-evidence base: `c5883ff7d72e754337895289628075d887ba37c2`.
This series was kept isolated while the coordinator froze checkpoint 3.

All four assigned once-loops were removed. The three pool updates now traverse
their actual slot arrays by index and invoke typed state callbacks directly;
all three explicit `sub_08249240` interworking-thunk calls were also removed.
Each complete unit independently matches the ROM under stock agbcc.

| Function | Source commits | Complete unit | Once-loops removed |
|---|---|---:|---:|
| `SpriteEffectController_UpdatePool` | `f34e49943`, then `df3c3e18e` | 144 bytes | 2 |
| `RouteSpritePool_Tick` | `bfdab71f1` | 144 bytes | 1 |
| `FeatherEffectPool_UpdateTask` | `b769f832a` | 140 bytes | 1 |

All three resulting units have zero focused errors and warnings. No compiler,
linter, suppression, dummy operation, replacement weighting loop, or new call
wrapper was introduced.

## Actual traversal and callback structure

The sprite-controller pool already has a shared 24-entry controller array. Its
old source wrapped the initial slot pointer in two nested do-once loops, then
advanced a typed slot pointer and a separate raw state-byte cursor. The accepted
source retains the existing precomputed array base and reads both the callback
state and callback argument through the same indexed element. A follow-up gives
the state table its actual pool/slot callback type and invokes it directly.

The route pool already has a shared four-entry actor array and
`RouteSpriteStateCallback`. Indexed member access replaces the setup wrapper,
integer state-address cursor, explicit byte stride, and post-loop pointer-range
workaround. The callback is invoked through its existing function-pointer type.

The feather loop's wrapper was around its occupied-slot test and callback,
inside the real twenty-entry loop. Its source comment explicitly attributed the
wrapper to autosolve's loop-depth weighting. It was not an initial-address
wrapper. The accepted update uses the actual twenty-entry `FeatherPool` and
typed `FeatherStateCallback`, preserving the existing array-base computation
and live occupancy read on every iteration.

No occupancy mask is cached across callbacks: later iterations still observe
changes made by earlier state callbacks. The initial empty/paused/locked guards
also retain their original behavior.

## Feather layout corroboration

The new `include/feather_effect_pool.h` contains the shared `Feather` and
`FeatherPool` definitions used by the updater. Its shape is established by
existing source, independently of the allocation experiment:

| Existing source | Layout evidence |
|---|---|
| `src/fn/sub_08128B4C.c` — allocator | Twenty slots, `0xA0` bytes per slot, pool array at `+0x3C`, slot index at `+0x93` |
| `src/fn/sub_0812975C.c` — destructor | Same twenty-slot array and stride, occupancy word at `+0x38` |
| `src/fn/sub_081297A4.c` — initializer | Task prefix, graphics storage at `+0x18`, sequence resource at `+0x34`, occupancy at `+0x38` |
| `src/fn/sub_081297DC.c` — creator | Complete task allocation of `0xCBC` bytes |
| `src/fn/sub_08128B9C.c`, `sub_081292D4.c`, `sub_081294B4.c` — animation states | Full feather field layout, including sequence storage, state, slot index, age, angle, and effect fields |
| `include/display_sequence.h` | Existing canonical sixteen-byte `DisplaySequenceState` matching the animation-state layout |

An ignored layout probe compiled with stock ARM agbcc measured:

| Property | Measured value |
|---|---:|
| `sizeof(struct Feather)` | `0xA0` |
| Feather sequence offset | `0x2C` |
| Feather state offset | `0x92` |
| Pool occupancy offset | `0x38` |
| Pool slot-array offset | `0x3C` |
| Slot count | 20 |
| `sizeof(struct FeatherPool)` | `0xCBC` |

At the original indexed-loop commit, the header had one dependent source unit,
`src/fn/sub_081296D0.c`; that complete 140-byte unit was independently checked.
Review then found incompatible private definitions at the four state callback
targets. The [interface follow-up](round14-feather-callback-interfaces_2026-11-16.md)
migrates all four to the owner and embeds the actual ContactSwarmSprite prefix.
The resulting six header dependents and the unchanged creator caller match.
The retained Scatter Step helper, broader scheduler callback contract and other
private family views remain explicit debt. The initial dependency inventory
and stock result remain preserved in `header-dependents.json`.

## Failed deletion and recovered source choices

Plain deletion keeps each unit's size but changes 12, 8, and 9 bytes respectively.
Merely replacing the separate state cursor with one advancing object cursor
changes the instruction shape. Indexing the original precomputed slot-array
base is the meaningful structural step: it preserves the real base lifetime
while letting the compiler derive its own state and entry induction variables.

For route and feather, indexing alone initially left the same 8-/9-byte register
differences. The compiler assigned the pool pointer priority 4,705 and the
synthesized state cursor priority 4,666, so they took the opposite saved
registers from the ROM. Diagnostic priority increases of 40 for those cursors
matched. Replacing the explicit thunk with the ordinary typed callback call
then matched under the unmodified compiler; no priority knob was adopted.

An early callback candidate that recomputed the array base inside the guarded
block still missed. The accepted form combines the original base lifetime with
the actual indexed callback operation. Changing the existing occupancy helper
to accept a typed pool did not fix the mismatch and was not adopted. These
nonmatching natural drafts remain in the evidence.

## Durable evidence and verification

[round14-pool-cursor-candidates_2026-11-16.jsonl](evidence/round14-pool-cursor-candidates_2026-11-16.jsonl)
contains 25 distinct complete source states for the three functions, including
the three final accepted states. The accepted feather candidate's patch also
adds its required shared header and records both file hashes. Every patch was
reconstructed independently against the evidence base and hash-verified.
Generated assembly and RTL are excluded.

Complete stock checks used the original extents in the first table, not a
candidate-length default. Focused before/after linter results and the header
dependency inventory are preserved under `build/round14/pool-cursor/`, together
with candidate files, the ARM layout measurements, and portable matchsearch
histories. The original pre-header graph histories and the later shared-header
and callback histories retain their separate compiler contexts. Combined
integration and full-ROM verification remain with the coordinator; this worker
did not run a full build or a whole-tree linter scan.
