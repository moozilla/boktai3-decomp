# Round 14: typed downed-state control transitions

Tags: #round14 #matching #once-loop #shared-types #checkpoint4

Date: 2026-11-16. These changes follow checkpoint three and are reserved for
checkpoint four. The representative failed drafts replay from checkpoint-three
base `f4989797f`.

Three downed/defeat callbacks now use the existing ChaseActor and
SceneActorChaseGroup fields and the shared SceneActorChase_SetControl operation.
All **89,664 bytes** of their complete source units independently match stock
agbcc. The focused compiler-inclusive scan falls from 49 warnings to 43, with
zero errors: **three once-loops and three constant locals are removed**.

| Source and callback | Exact complete unit | Source commit |
| --- | ---: | --- |
| `dog.c`: Dog_UpdateDefeatDelay | 37,088 bytes | `48aef5318` |
| `golem_main.c`: Golem_UpdateDying | 20,620 bytes | `2cf9fe4cd` |
| `skeleton_main.c`: Skeleton_UpdateDownedState | 31,956 bytes | `37dca1272` |

Header commit `500ab34a5` corrects the owned Skeleton_UpdateDownedState
declaration to accept ChaseActor. It is paired with the source definition change;
there is no layout or runtime calling-convention change. **Both skeleton.h
dependent units independently match.**

## Recovered operation and fields

All three callbacks perform the same sequence. They consume the control-entry
flag, select the directional animation, and set motion flag `1` if flag `2`
is clear. Every 60 frames they refresh the category's response slot. Once the
class timer is nonpositive and status flag `0x1000` is clear, they install control
kind six with their class-specific completion callback. Skeleton selects
animation eight; Dog and Golem select animation six.

The shared types already provide the fields used by those operations:

| Previous address calculation | Existing shared field |
| --- | --- |
| actor + `0x2B2` | `base.field2B2` |
| actor + `0x2D4` | `base.motionFlags` |
| actor + `0x98` | `base.category` |
| group + `0x1F2`, at actor + `0x5C6` | `group.field5C6`, read as signed |
| actor + `0x2D0` | `base.statusFlags` |

The old code reused one offset variable for the timer and status addresses, then
used the masked status result named `zero` to clear several packet fields. The
accepted code deletes that temporary and tests the status field directly.
SceneActorChase_SetControl performs the actual packet initialization and pending
control reset through named fields. No replacement alias, weighting construct,
new helper, or ignore is introduced.

## Natural alternatives and remaining source choice

Removing only the one-shot wrapper changes 23 bytes in each complete unit. The
existing shared setter first supplied an exact replacement for the whole packet.
Migrating the surrounding raw accesses to the real actor and group fields then
remained exact, including deletion of the masked-status temporary.

A broader simplification of the motion-flag predicate was also tried in Dog and
Golem. It produces complete units four bytes shorter than the originals. The
accepted source retains the ordinary cached flag value and mask operations; the
failed substitution does not, by itself, establish that those named values were
artificial. No extra block or assignment was added to recover the four bytes.

`evidence/round14-downed-control-candidates_2026-11-16.jsonl` preserves the three
plain-deletion drafts and the two broader typed-predicate drafts. Every record
contains its complete source patch, baseline and candidate SHA-256, and stock
check result. **All five patches replay and hash-verify.** Reconstruct each as an
independent draft against its named checkpoint-three source/toolchain context;
the records form a candidate catalogue, not a patch series.

The final scoped counts are one once-loop, two statement expressions, 33 constant
locals, six address/dereference expressions, and one local union. Remaining
warnings in these units stay visible. This note reports the accepted family and
its checks; the coordinator owns the combined tree census and ROM build.
