# Grid attack actor unit consolidation

Tags: #grid-attack-actor #unit #matching #080297E8 #0802C678

Range: `080297E8–0802C678` (end exclusive). Subsystem interpretation is tentative.

Consolidated all 63 existing matching files into `src/grid_attack_actor.c`, in
ROM order, with the six existing unmatched functions in their original positions.
No new function gaps or inventory entries. No commits or shared-doc edits.
`merge_unit.py --write` performed the merge; its `git rm` removal step was replaced
at invocation time with filesystem unlinking because this worker must not write
Git metadata. The tool itself was not changed.

## Layout

`include/grid_attack_actor.h` defines the shared 0x924-byte actor, its transform,
word-copy position, packed vector, outgoing message packet, animation resource
header, and drawable. It reuses `ContactCollider`, `SceneEventReceiver`,
`DisplaySequenceState`, and `SceneVec` from existing headers; engine declarations
come from `engine.h`, without local redeclarations. The actor's common state,
action, callback, damage, attack, palette, collider, linked-object and message
fields are unified rather than kept as per-function partial views.

The old broad animation views included purported scale fields at +0x343 onward.
They were unused by this unit; initialization actually fills +0x1C4..+0x3C4 with
sixteen palette banks. The header therefore describes that storage as a palette,
not as the unrelated animation-view template. +0x3C4 and +0x5C4 hold the two
additional palette buffers. The header at +0x86C contains its palette pointer at
+0xC (+0x878 in the actor).

Names and semantic field interpretations remain provisional. New file-local
aliases describe damage collision, targeted projectile spawning, message
processing, animation/target updates, collider/palette updates, initialization,
and the previously unnamed wait action at `0802B3E4`.

## Matching adaptations

Four consumers retain a typed pointer to the transform at +0x48. Direct actor
field access removes a live base register and changes the original instruction
sequence. Particle emission likewise retains a typed drawable-base pointer;
accessing its position through that pointer preserves the original lifetime.
Packed bitfield vectors preserve the original stack writes; position copies use
two word fields. State and action helpers retain their distinct original forms
where differing constant order or byte-return flag materialization matters.
Task update/destroy callbacks use explicit API casts because their original
implementations return values ignored by the task engine. No engine prototype
correction was needed.

Existing register pins/barriers were retained, with the original concession
markers on the affected functions. `depin.py --write` found all six remaining
hints necessary; no new pins or barriers were added.

## Unmatched attempts

All six functions received typed C attempts. Only the close collision draft is
retained in production source, guarded by `NONMATCHING` and scored with
`tools/check.py --nonmatching --record`:

| Function | Result | Remaining issue |
|---|---|---|
| `08029870` | 97.1% guarded draft | Two bytes differ: signed-x load offset uses r1 instead of r3. |
| `08029BF4` | 29.7%, assembly retained | Stack word copies and random-index register allocation. |
| `0802ADBC` | 67.7%, assembly retained | Event receiver loop, switch setup, action constant/register order. |
| `0802B7C8` | 64.9%, assembly retained | Inlined packed animation stepper, fixed-point shaking, target loop. |
| `0802BAFC` | 56.2%, assembly retained | Collider flag flow and palette blend register allocation. |
| `0802BFAC` | 69.2%, assembly retained | Large script initializer: stack slots, constants and callback setup. |

Discarded low-score attempts are available locally in ignored
`build/grid_all_drafts.c` and `build/grid_all_drafts.h`; they are experimental,
not complete matching C. In particular, their target loops need an array-layout
review before reuse. The best retained collision attempt used about five focused
checks; the other attempts were screened once after compilation fixes. Roughly
25 manual checks and 14 automated depin checks covered consolidation and drafts.

## Validation

The merged default unit checked byte-identical across all 11,920 bytes. An ARM
layout compilation checked the actor size and 17 key offsets. Formatting and the
complete build are recorded below after completion.

Exact code coverage in this range stays **62.617% (+0.000 percentage points)**:
63 matching C functions, 7,464 bytes. No newly matched functions. The collision
draft is excluded from exact progress. Repository-wide matched bytes must remain
unchanged; totals remain in `PROGRESS.md`.

Final validation: `make format` completed; `depin.py --write` retained six of six
existing hints; `tools/build.py` printed **`build/boktai3.gba: OK`**. Local progress
confirmed no matched-byte decrease. The first complete build encountered missing
worker-local resource extracts under `build/res/`; 1,136 required files were
extracted from the ROM and the existing generated assembly's filenames were
recreated locally. The resumed build passed, without changing shared `gen/`,
source tools, or symbol inventories. Generated files remain ignored.
