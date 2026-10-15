# Scene actor reaction callbacks

The contiguous cohort **080800A4–08081290** contains seven complete callbacks:1,008/980/284/364/240/980/732 bytes, totaling4,588 bytes. They consume the prepared flag2B2 and manage actor reaction, route, drawable and animation state. Names remain tentative; this analysis does not claim runtime coverage.

The existing callback8005C is72 bytes and receives no new credit. The true next entry is800A4, beginning PUSH r4-r7/LR and saves of SL/SB/R8; the old800AC label is the second, interior push. Its complete body restores both saved sets and ends80494. Parent metadata already records the corrected entry and rejected interior label; this worker does not duplicate that bookkeeping commit.

The narrow header `scene_actor_reaction.h` reuses `SceneActorRegistryResetView` and describes the group's collider pointer at3F8, route control at420, and destination pointer5D4. Group414/418 and route control counters retain their observed widths. These are phase views of the shared actor, rather than evidence of a separate allocation.

## Exact prefix work

The current exact set is80868/80984:648 new bytes (+0.0268 percentage points), both with minimized register concessions.80868 matches its complete284 bytes (depin18→5 hints). On the prepared event it clears flags2CC masked141C0 and route control counter438, value434 and timer428, then calls764C8(actor,3,1). Category1 plays sound253, category3 delegates77848(actor,1), category4 plays sound212. It disables colliderA flag4, clears its parameter178, shows the drawable, resets animation to state3/phase0, clears attachment valueAA, sets drawable blend/type byte7 or3A to3, sets flags2D0 bit80000, and disables the shadow at320.

The target's event branch fits a short Thumb branch. Redundant empty barriers initially made agbcc estimate it too long and shift literal pools. A near-aware removal pass shortened that branch; moving initialized state3 before its first field-address construction finished the full match. All asm operands are initialized, and no explicit r7 binding or handwritten instructions are present.

80984 matches all364 bytes. It clears the same route control state, then categories1/4 play sounds253/212 after selecting animation3; categories3/5/6/7/21 play318, category25 plays459, and other categories skip the sound. It clears primary parameter178, shows the drawable, resets state3/phase0 and clears attachment valueAA. Unlike80868, it does not set the collider-disable flag or alter drawable blend/shadow state. Its initialized timer-zero lifetime also supplies the sequence mask~1; the empty barriers preserve a subtract-from-zero form without reading an uninitialized value.

## Remaining contiguous work

800A4 has a full private semantic draft,984 bytes against its reviewed1,008 bytes at44.51% fuzzy. Entry clears reaction/route state, shows the actor and enables its shadow. A pending route flag selects a defeated control when health≤1; otherwise it copies the route450 position into destination5D4, derives terrain height and slope, tests21F204/21F7A8, and installs fallback7FC90 or route7FDC0 with kind14 and an embedded command at460. The entire body remains uncredited pending matching refinement.

80494/80AF0/80BE0/80FB4 are reviewed whole inventory entries awaiting semantic C in this worker. Earlier actor behavior gaps78BB4 and78EE8 remain private, with their evidence in `SCENE_ACTOR_BEHAVIOR_FRONTIER.md`.
