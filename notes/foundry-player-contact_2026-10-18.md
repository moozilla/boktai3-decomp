# Foundry player/contact worker, 2026-10-18

Tags: #player #collision #foundry

Candidates remain in `wip/foundry/`; per-function records are in
`build/foundry/FUNC/result.txt`. Nothing was committed or promoted to production.

- 0823D00C: exact in one check; Player_SetActionOrigin copies position and applies facing-dependent offsets using existing Player/SceneVec fields.
- 0823D714: exact in one check; Player_SaveActionIndex stores the optional action index at C0D and its one-based variant at AC9. Only padding was replaced in player.h; all 16 matched consumers passed hdrcheck.
- 0823D7E0: 95.7% after ten checks; Player_CollectCallbackFlags ORs three optional callbacks. Closest draft has unsigned rather than signed loop branch. A signed goto version reached CORE zero but swapped register allocation; signed structured loops became descending counters.
- 0823DDE4: exact in three checks; SceneVec_OffsetPolar adds scaled sine/cosine offsets in X/Z and preserves Y. The inline scaling idiom reproduced the register lifetimes.
- 08242F7C: exact in two checks; Player_FindMaskedSlot selects the first set player-mask bit or defaults to slot zero. A shared return label fixed layout.
- 082433C8: exact in two checks; Player_ApplyGatedContact consumes/sets collider response bits before calling 08243090. Added ContactCollider.responseFlags at 46 by replacing padding; all 22 matched consumers passed hdrcheck.
- 08243678: 70.5% after five checks; Player_UpdateResetBlink has CORE zero with the player/timer registers exchanged. Several lifetime/layout variants did not improve allocation.
- 08249238: a real bx-r0 callback trampoline, not a split tail. The one-check C draft emits unresolved `_call_via_r0`; the result record's 0.0% means no accepted/scored candidate, not a measured fuzzy score.

All names are provisional. The existing worktree had unrelated header and documentation edits; these were preserved.
