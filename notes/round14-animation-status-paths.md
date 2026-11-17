# Round 14: actor animation fields and the sequence-command holdout

Tags: #matching #animation #concessions #input

## Result and scope

This bounded follow-up starts from worker commit `5c3596031`, after the
17-unit sequence-view/background batch. It investigates the remaining
ordinary animation reads in `ActorController_UpdateAnimation` (`08184F04`)
and `Anim_SetSequence` (`08220C8C`). The input function `0805E7D8` received a
writer/sibling review, with no new source experiment.

**One source fix landed; `Anim_SetSequence` remains open.** Commit
`65f5d8bde` uses the existing shared actor layout and removes the unjustified
status-byte volatility from `08184F04`. An independent stock
`tools/check.py src/fn/sub_08184F04.c --target-size 228` check reports zero
differing bytes across the entire unit. The compiler-aware concession scan
changes from one `volatile-view` warning to zero warnings and zero errors.

No shared header, ignore, compiler setting, or control flow changed. The
worker ran no full build and made no progress-report change. This source fix
is separate from the previously delivered checkpoint-3 batch.

| Function | Volatile-view warnings before | After | Outcome |
| --- | ---: | ---: | --- |
| `ActorController_UpdateAnimation`, `08184F04` | 1 | 0 | Shared actor fields; 228 stock bytes exact. |
| `Anim_SetSequence`, `08220C8C` | 2 | 2 | Ordinary command data; source unchanged, natural candidates remain nonmatching. |
| `SceneInput_GetNavigationKeys`, `0805E7D8` | 1 | 1 | Read-only follow-up; prior input candidate and once-loop remain open. |

The prior audit's open entry for `08184F04` is superseded by this result.
The other entries in `notes/round14-volatile-view-audit.md` retain their
separate conclusions about ordinary data, RFU interrupt state, and DMA.

## Actor animation: use the existing owning object

The previous ordinary-byte and widget-only candidates produced 224 or 220
bytes instead of 228. The actual owning type already exists in
`include/scene_combat_actor.h` and exposes every actor field used here:

| Actor offset | Shared member |
| --- | --- |
| `0xD1` | `animationStatus` |
| `0xB32` | `flags` |
| `0xB3F` | `action` |
| `0xB42` | `actionCounter` |
| `0xB43` | `animationComplete` |

The accepted source takes a `SceneCombatActor *`, uses those members, and
uses the existing `SceneInputWidget` for the animation at `+0xB4`. The status
member is an ordinary `u8`. This removes the volatile read, its temporary
byte pointer/value, the unused private status bitfield, the private
three-word animation view, the unused `TestFlag` helper, and an obsolete
allocation comment. It adds no replacement helper or layout. The resource
and widget addresses at `+0x94/+0xB4` remain explicit because the shared
actor does not currently expose those entire embedded objects.

The status meaning is supported by actual writers. `Anim_Step`
(`08220F70`) sets bit 0 when the last frame is one tick from completion,
bit 1 when the sequence ends, and bit 2 when it advances a frame.
`Anim_SetFrameDurationScale` (`08220F00`) recomputes bit 0 after changing the
duration. `DisplayAnimation_TestFinished` and
`DisplayAnimation_GetFrameBoundary` in `src/display_animation.c` consume
bits 1 and 2 respectively. No asynchronous status writer was established.

A scratch representation with three named status bitfields was also tested.
The unpacked definition failed its one-byte size assertion; the correctly
packed definition produced 224 bytes and did not match. Neither is landed.

The ordinary status-or-ended completion expression in the motion siblings
`08074EB0` and `0807A8CC` was tested with the shared actor fields. It retained
the 228-byte extent but changed 53 bytes. The accepted source therefore
preserves the two existing completion tests and their order.

## Sequence commands: real format recovered, allocation still unresolved

The earlier log `notes/depin-manual-4a_2026-10-21.md` explicitly records that
`Anim_SetSequence`'s reused byte pointer and volatile command cast came from
a permuter. Those forms remain source debt; they are not evidence for an
interrupt-managed command stream.

The new hypothesis models the resource itself. Callers such as
`DisplayAnimation_InitializeSequence` obtain it from
`ScriptProc_Start(0x922E, sequenceId)`. This is the `.anm` archive, distinct
from the `0xCB05` keyframe package decoded by `tools/assets/sprites.py`.
The sequence function reads a four-byte entry per sequence starting at
resource offset 4: an unknown byte, a frame-count byte, and a halfword byte
offset. Rows contain `frameCount` halfword commands. The existing
`DisplaySequenceState` and playback helpers establish the command fields:
duration weight in bits 0–3, flips in bits 4–5, and frame number in bits 6–15.

`tools/studio/atlas.py:speaker_portraits` already reads the resource's
sequence count as a halfword at offset 0. A read-only check of all 75 `.anm`
resources supports a second halfword command count at offset 2: their sizes
equal `4 + 4 * sequenceCount + 2 * commandCount`, followed by four bytes in
41 resources or six bytes in 34 resources. Every sequence entry's command
range is inside its resource. The command-count interpretation is inferred
from those extents; the first entry byte and trailing bytes remain unnamed.

The resulting scratch candidate uses those two counts, typed four-byte
entries, ordinary `const u16 *` commands, the canonical playback state, and
the existing `DisplaySequence_ReadCommand` API. It removes the permuter's
byte-pointer reuse. It produces 148 bytes, the correct full-unit size, with
five differing operand bytes in five instructions:

* At `08220CAE/08220CB0`, the initial row-address addition/store uses `r1`
  where the ROM uses `r0`.
* At `08220CFE/08220D00/08220D02`, the command-address addition, halfword
  load, and mask constant use the other operand registers.

The register-blind instruction shape matches. Scope identifies a local
allocation preference in the initial pointer addition and a regmove tie at
the final mask. This pass did not add a use, alias, branch, union, or loop to
steer either decision. No compiler override is part of any trial result.
The previous shared-read candidate's narrower two-register mismatch and
diagnostic override remain recorded in the earlier audit; they are not a
source solution.

A second format hypothesis used the actual command bit widths as a packed
two-byte record. It produced a byte load and shifts instead of the ROM's
halfword load and mask, changing six bytes. The unpacked form failed its
two-byte size assertion. Neither form is adopted. No new shared command
type is justified by these nonmatches.

Both production volatile-view warnings in `08220C8C` remain visible.

## Input review: keep the ordinary sampled state distinct

`src/runtime_input.c` establishes the normal writer:
`TaskSystem_UpdateRoot` calls `Input_UpdatePadStates`, which reads hardware
or synchronized link inputs and writes the shared `RuntimePadState`
`held`, `pressed`, and `released` fields. The hardware register read itself
does not establish interrupt ownership of the resulting RAM snapshot.

`SceneInput_ReadRepeatingDirections` (`0816B3D4`) provides a natural sibling
for the navigation repeat algorithm. The earlier ordinary-array,
direct-global, and typed-pad candidates for `0805E7D8` each produced 64
bytes instead of 72. No new shared layout or call path emerged in this
follow-up, so those spelling experiments were not repeated. The local
volatile pointer and once-loop stay open; no ignore or shared volatile
declaration was added.

## Replayable evidence

`notes/round14-animation-status-paths-trials.jsonl` contains ten complete
records: two baselines, the accepted actor source, five compiled nonmatches,
and two explicit compile failures. Each record includes its hypothesis,
source hash, measurements, and a full patch against the named source file
at `5c3596031`. Every patch was replayed against that baseline and its
resulting source hash checked.

`notes/round14-animation-status-open-candidate.patch` contains the final
typed `.anm` resource candidate. It is unapplied and passes
`git apply --check` against the current source. To reproduce a record,
materialize the baseline file, apply its embedded patch, and run the stock
per-unit check with 228 bytes for `08184F04` or 148 bytes for `08220C8C`.
Compile-failure records intentionally fail their layout assertions.

Transient trial objects, scope output, and the accepted unit's independent
check and compiler-aware before/after logs are under
`build/round14-animation-paths/` and `build/check/`.
