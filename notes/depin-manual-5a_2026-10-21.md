# depin-manual-5a worklog (2026-10-21)

List wip/depin_list5.tsv; stopped early at coordinator request after 4 clean results.
depin.py --write found nothing on any of the 40 files. Permuter ran (150 s, -j 1) on stripped copies for the first ~14.

## Clean
* ContactImpactEffect_UpdateContact (081C3E04): the `zz asm("r3")` pin was unnecessary; plain `s32 x, zz;`.
* RecordPanel_DrawTime (0810D688): permuter. Reuse the dead `mask` local: `seconds = 999; mask = seconds; fraction = mask;`.
* SceneActorRegistry_FindNearestCategoryActor (08070D24): permuter. Inline helper IsUnavailable returns `s16` instead of `u32`; all pins/asm removed.
* Storage_BeginCategoryMode (081BF440): permuter. `int new_var;` set to -1 right after `p->mode = mode;`, used as the call argument (`none = new_var`).

## Not solved
* ActorAttachment_TickLandingBounce: unpinned is 2 diffs (`adds r0,r0,r1` vs `adds r1,r1,r0` for height+z); permuter found nothing.
* ActorDecision_UpdatePulsePhase: unpinned 2 diffs (calls need the `s` copy in r0); permuter found nothing.
* Permuter best scores: RunAlternateTimedAction/RunTimedAction 70, ConsumeLiftFlag 140, ScriptLower 670, SceneHud_TickWidgetSlides 40, ScenePanel_TickRig 145, RequestScriptState 345, ProximityEmitter_TickActive 470 (not ported/checked).
* Remaining list entries untouched.

## New techniques
* Changing an inline helper's return type (u32 -> s16) alters codegen enough to drop pins.
* Introducing `int new_var = const` early and using it later as a call argument.
