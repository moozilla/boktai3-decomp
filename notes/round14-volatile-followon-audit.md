# Round 14: remaining sequence views and background queue

Tags: #matching #animation #backgrounds #tooling

## Scope and result

This pass covered the 17 additional units assigned after
`notes/round14-volatile-view-audit.md`. Their source baseline is `1a35f0f22`,
merged into the worker before editing. Earlier address, statement-expression,
and constant-local changes are preserved.

**All 17 assigned volatile-view warning lines are resolved.** They had three
different causes:

* Eleven private sequence layouts duplicated the shared
  `DisplaySequenceState`. Their callers already used the ordinary shared
  playback implementation. Replacing the private member types also removes
  redundant casts and corrects the local restart parameter where present.
* Four private `FanSeq` definitions were entirely unused. They are deleted;
  their warnings did not correspond to emitted volatile accesses.
* Two functions still had live volatile source forms: the background command
  count and the homing effect's copied playback implementation. Both now use
  ordinary dataflow and match exactly.

The homing playback change also removes two volatile flag casts that the
current scanner did not report. They are recorded as source improvements,
without inflating the reported warning reduction.

| Kind, within these 17 units | Before | After |
| --- | ---: | ---: |
| `volatile-view` | 17 | 0 |
| `once-loop` | 1 | 1 |
| `const-local` | 5 | 5 |
| `addr-deref` | 1 | 1 |
| Errors | 0 | 0 |
| Total warnings | 24 | 7 |

The before/after scans include compiler diagnostics. No warning kind or site
was added, and no ignore, shared header, or compiler setting changed.
**Independent stock whole-unit checks passed for all 17 units, containing 38
functions and 20,620 reviewed bytes.** The background unit contains 22
functions; every other unit contains one. The worker ran no full build and
made no project progress-report change.

## Accepted commits

Commit IDs below refer to the integrated delivery branch. Each commit changes
one translation unit and removes one reported warning line.

| Unit | Source improvement | Whole-unit bytes | Integration commit |
| --- | --- | ---: | --- |
| `src/background_controller.c` | Copy the actual queue arguments with the shared source/destination index. | 5,372 | `03ef42221` |
| `0812A7B8` | Use shared playback state for linked-effect removal. | 368 | `afabcb494` |
| `0812AA0C` | Use shared playback state for rising linked effects. | 676 | `bead12fa8` |
| `0812ACB0` | Use shared playback state for linked-effect tracking. | 980 | `84822ec4c` |
| `0818EF64` | Use shared playback state for moving Nidhoggr effects. | 408 | `eb0a95ebe` |
| `0818F0FC` | Use shared playback state for panel effect lifetime updates. | 284 | `8f79dd568` |
| `0818F444` | Use shared playback state and restart parameter for Nidhoggr burst creation. | 200 | `72cefd594` |
| `0818FEE4` | Use shared playback state for seeking Nidhoggr effects. | 912 | `96d9affad` |
| `081A39E0` | Replace the copied animation implementation with the existing shared playback API. | 800 | `ca40079a8` |
| `0820212C` | Use shared playback state for scripted combat actors. | 1,244 | `a6de537e2` |
| `08204230` | Delete the unused `FanSeq` declaration. | 1,236 | `3765389d0` |
| `08209AC4` | Use shared playback state for fan cannon updates. | 2,052 | `cfb850f7e` |
| `0820A5CC` | Use shared playback state for fan cannon creation. | 1,392 | `bb2d07238` |
| `0820B3E0` | Use shared playback state for dragon puppy combat. | 1,012 | `56a9ddc95` |
| `0820B7E0` | Delete the unused sequence declaration from dragon puppy setup. | 1,280 | `ca1fa3707` |
| `0820E724` | Delete the unused sequence declaration from poison hand setup. | 1,156 | `1789e3684` |
| `0820F830` | Delete the unused sequence declaration from orange spine setup. | 1,248 | `4ca23f162` |

## Background command arguments: recover the real array induction

`Background_QueueCommand` is at `080021DC`, with an 88-byte reviewed function
extent inside the 5,372-byte background unit. Its old form copied the type,
length and input pointer into aliases, made the queue count volatile, and
constructed each destination with byte arithmetic. A comment explicitly
described the desired reload and operand allocation.

The actual layout already exists in `include/background_controller.h`:
`BackgroundCommandQueue` holds a count, eight type tags and eight command
records; each record contains eight signed-halfword arguments. Both normal
and scrolling backgrounds use that queue. The queue is populated by normal
script/task code and drained by the corresponding update path; no interrupt
writer was established.

The first ordinary count-pointer candidate changed 37 bytes in the whole
unit. Direct queue fields with the old loop control flow changed 40 bytes.
Using the neighboring `ScrollingBackground_QueueCommand` loop with
`*args++` preserved the ROM's destination indexing and count reload but still
changed five instructions. Scope identified different lifetimes and
allocation for the argument pointer and count-address value: the source
argument remained in `r7`, whereas the ROM saved the incoming base in `ip`
and used a separate advancing pointer in `r6` inside the loop.

A common argument-copy helper for both background families was tested, but
it also changed the scrolling function and made the unit four bytes larger.
That helper was not added.

The natural indexed source `args[j]` resolves the difference. The destination
is `p->queue.commands[p->queue.count].args[j]`, so both sides express the same
real argument index. The compiler derives the advancing source pointer from
the unchanged input array base. The result matches the ROM exactly, including
the original count reload, without volatility or an explicit duplicate source
pointer.

The final function removes six ancillary aliases, the raw byte-offset store,
and the allocation commentary. It uses an ordinary `for` loop and the actual
queue fields. The separate scrolling-background implementation is unchanged.
The unrelated once-loop in `Background_FindMapId` and both constant locals
remain visible.

## Homing playback: reuse the existing operation

`EffectPool_UpdateEntry` (`081A39E0`) had a private `Animation` layout with
different names for the same sequence fields. Its `Animate` helper copied the
common playback algorithm, including a volatile command pointer and two casts
that read the flag byte as volatile. A private `MaskFlags` helper handled the
narrow flag values.

The entry's sprite prefix has flags at offset 0 and the frame at `0x10`; its
playback state is the established 16-byte sequence layout at `0x2C`. The
accepted source uses `DisplaySequenceState` and calls the existing
`PlayerCue_StepSequence` operation for that sprite/state pair. The obsolete
mask helper and competing collider declarations are removed. No new playback
helper or control union was created.

The entire 800-byte unit matches. Its unrelated address-taken local mask
expression and constant zero remain warned; the earlier source cleanup is
preserved. The warning reduction is one line, while the actual source removes
all three volatile views in the old copied playback body.

## Private sequence layouts: distinguish source clutter from live accesses

The eleven used private sequence declarations had the same offsets and size
as `DisplaySequenceState`. Their playback calls already passed those objects
to the shared API, often through a cast to the correct type. This pass makes
the stored member and local sequence pointer agree with that API. It does not
attribute a runtime volatile-access removal to a declaration bypassed by the
live implementation.

The four unused `FanSeq` definitions occur only at their declarations in
`08204230`, `0820B7E0`, `0820E724`, and `0820F830`. Their initialization code
already uses the shared sequence API directly. The full unused definitions
are removed instead of retaining them with a different qualifier spelling.

`0818EF64` also loses an obsolete comment claiming current register-pin and
empty-assembly concessions. That comment was attached to the removed private
sequence declaration and no longer described the source.

## Verification and remaining scope

The independent checks compiled the final source paths with the stock
compiler and linked each complete translation unit against the pinned symbol
snapshot. Every result used `tools/check.py --target-size` with the extent in
the table, and every byte matched. The helper experiments were scratch
candidates only.

`notes/round14-volatile-followon-trials.jsonl` preserves 38 baseline and
candidate records with complete patches against `1a35f0f22`, hypotheses,
hashes, extents and match results. Transient compiler-aware scans and
independent check results are under `build/round14-volatile-followon/`.

This assigned queue has no remaining volatile-view warnings. The earlier
audit's six warnings in five different units remain open or classified as
legitimate IRQ/hardware access, as documented in
`notes/round14-volatile-view-audit.md`. The three root-owned file-level
volatile errors and other source-debt categories are not resolved by this
follow-on pass. No annotation was added; the separately source-backed
libgcc `umul_ppmm` once-loop annotation remains outside these counts.
