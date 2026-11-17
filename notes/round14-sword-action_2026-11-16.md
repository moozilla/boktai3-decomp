# Round 14: sword component action recovery

Tags: #round14 #matching #once-loop #shared-operation #checkpoint4

Date: 2026-11-16. Candidate source baseline: checkpoint three, `f4989797f`.

The horizontal and depth wave-return callbacks now use a shared action setter.
Both complete **228-byte units match stock agbcc**, and **both header dependents
match** after the helper is moved into `sword_wave_component.h`. The focused
compiler-inclusive scan falls from **zero errors/four warnings to zero/zero**:
two once-loops and two constant locals are removed.

| Concern | Commit |
| --- | --- |
| Shared SwordWaveComponent_SetAction operation | `206ddbc93` |
| Sword_UpdateHorizontalWaveReturn, `080F4FA8` | `f3a13c05a` |
| Sword_UpdateDepthWaveReturn, `080F508C` | `2fccf50dc` |

## Source evidence

Each callback runs the same end-of-flight transition when its timer reaches zero.
It restores rotation to 128, activates the component with action one, raises the
action-entry flag, and installs IvyChild_UpdateAfterBob. The accepted helper
represents the three related action fields: write `active`, write the requested
action to `phase`, and OR activity into `flags`. Rotation and the callback remain
direct assignments at the caller.

This operation is already established elsewhere in the component family:
ArmorLaunchState_SetAction in `armor_part_queue.h` writes the corresponding
activity, action, and entry-flag fields; Armor_UpdatePartOrbit (`0818CB30`) selects
actions two through four with the same three stores. Sword_AccelerateReturnTurn
(`080F5620`) and ActorPhase_ApproachNeutral in `ivy_child.c` also write the same
action fields before selecting IvyChild_UpdateAfterBob. Their surrounding motion
and callback layouts are not assumed interchangeable.

The helper uses actual byte-valued action parameters and has two callers. It
contains no one-shot loop, replacement constant local, extra copy, or ignore.
The existing SwordWaveComponent layout is unchanged. No callback prototype or
other family's header is changed.

## Alternatives and validation

Removing only the wrapper leaves a 228-byte unit but changes ten bytes in either
callback: the action value and field-address temporaries exchange `r0` and `r1`.
Direct literal field assignments remove both warnings but produce 232 bytes;
they also change reload choices and lose a reused immediate. Chaining the two
byte assignments has the same result.

A complete fixed bobbing-transition inline or macro likewise produces 232 bytes.
A broader setter taking the callback as another parameter restores the 228-byte
length but still changes 189 bytes. Separating the actual three-field action
operation from rotation and callback installation is what matches both axes.
The accepted source uses no compiler override.

`evidence/round14-sword-action-candidates_2026-11-16.jsonl` contains **14 complete
source drafts**: twelve failed alternatives and two exact action-setter drafts.
Every patch replays and hash-verifies against the named baseline, with stock
check results recorded. The two exact snapshots keep the helper beside each
function for independent reconstruction; the final source shares its single
definition in the owned header. These records are independent drafts, not a
patch series, and contain no generated assembly or ROM data.

The coordinator owns integration, combined counts, and the full ROM build.
