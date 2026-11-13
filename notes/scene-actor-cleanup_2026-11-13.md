# Scene actor cleanup, 2026-11-13

Tags: #actors #scene #matching

The worker stopped at the owner's end-of-run instruction after finishing Actor Phase.
Fourteen of the 59 assigned ranges were consolidated (23.7% of the cleanup queue),
replacing 235 one-function files. Matching coverage gains 0.0 percentage points: all
these functions matched before cleanup. The consolidated code totals 31,796 bytes.
Every new unit matches with the stock compiler, without new concessions.
The coordinator still needs to validate the combined ROM build.

## Consolidated units

| Request | Unit | ROM range | Functions | Bytes |
| --- | --- | --- | ---: | ---: |
| 0002 | `src/scene_actors.c` | 080213EC–08021EA8 | 27 | 2748 |
| 0003 | `src/actor_pool.c` | 0802C678–0802C8A0 | 9 | 552 |
| 0004 | `src/pooled_actor_helpers.c` | 0802C8A0–0802CD04 | 17 | 1124 |
| 0006 | `src/pooled_actor_controller.c` | 0802CD04–0802D5B4 | 28 | 2224 |
| 0007 | `src/scene_actor_lifecycle.c` | 0802D5B4–0802D9F8 | 20 | 1092 |
| 0014 | `src/scene_actor_animation.c` | 0802D9F8–08030710 | 52 | 11544 |
| 0009 | `src/contact_actor_display.c` | 080427FC–08042E18 | 9 | 1564 |
| 0010 | `src/pooled_motion.c` | 080454C0–08045A20 | 19 | 1376 |
| 0015 | `src/contact_sequence_actor.c` | 08045D18–08046880 | 22 | 2920 |
| 0011 | `src/orbit_contact_actor.c` | 08046880–08046B28 | 8 | 680 |
| 0012 | `src/contact_burst.c` | 08047F68–080483A0 | 9 | 1080 |
| 0013 | `src/scene_actor_terrain_reactions.c` | 08072854–08072BA8 | 3 | 852 |
| 0016 | `src/scene_actor_profiles.c` | 08073C04–0807462C | 6 | 2600 |
| 0017 | `src/actor_phase.c` | 080F8010–080F85B0 | 6 | 1440 |

## Types and compiler evidence

- Shared StateActor fields now describe health, registry slots, targeting, animation, callbacks and effects. All 23 header consumers matched. An early-return search retained the ROM register allocation; direct health-field pointers preserved required reloads.
- PooledActor now shares its body, colliders, action queue and display state. Word-sized direction arguments are narrowed at the established storage boundaries; all 73 API consumers matched.
- SceneActor and its manager share resource descriptors, route/probe state, interaction configuration, packets and display variants. The two entered flags are distinct. Explicit keyframe pointers and unsigned route-coordinate reads preserve the compiler address grouping. The final broad check passed all 2,203 current consumers and renamed callers.
- The existing scene_actor_animation.h was restored after a filename collision; the new action interface is scene_actor_actions.h. No failed intermediate header remains.
- The lifecycle callback loop uses an exit return after four slots. Scope identified dead-counter loop reversal as the obstacle; setting the counter before the callback pointer preserves the target allocation.
- Contact display uses existing graphics/collider types. DestroySlot retains its ignored unspecified result after the void removal call: changing its return type changes the epilogue register. The one-pass health-contact assignment retains its original grouping because flattening it swaps allocated registers.
- Motion construction no longer needs its goto, and its signed scale helper is shared. Sequence and burst pools use shared resource and collider types; pool task prefixes use Task. The sequence inactive search retains its original exit branch because an early return changes the epilogue.
- Profile initialization uses SceneActorProfileEntry fields. Object sizes and entry stride use sizeof. Entry address arithmetic remains integer arithmetic to preserve the lookup-miss behavior without deriving a pointer from null. A structured default-search loop changes branch layout and was not retained.
- Actor Phase reuses SceneActorRegistryResetView, ContactCollider, ActorSprite and Player. ActorPhaseContext names trail origin/angle/speed, the auxiliary sprite, animation workspaces, shadow, relocation delay and flags. Five state transitions share SetPhase. The eight-byte position copy includes the sprite position tail padding. Rewriting the reverse-flicker exit as an early return changes instruction layout; its original visible label remains. All six original functions and the final unit match.

## Tool changes

- Request 0001: merge_unit writes the merged output before unlinking source inputs and leaves git staging to its caller. Four tests cover removal and in-place output, including untracked inputs.
- Request 0008: unit_layouts measures local struct/union fields with stock-compiler probes, including nested members and bitfields. Two compiler-backed tests pass. The reused layoutmerge probe reader now closes its resource.

## Deferred work and restart point

- 08024F2C–080297E8: related Vampire/action/reaction ranges need one concrete object layout. An oversized player/position union crosses callback/message fields; the initializer at 08028F78 is useful recovery evidence.
- 08030710–080308BC: the typed manager candidate matches only while preserving previous-minute-pointer subtraction. Replacing it with the named minuteChanged field, including an explicit field pointer, changes address formation and register lifetime. The unrelated singleton reset at the range end matches without its old compiler override. No manager candidate was landed.
- 0806F990–0806FAD4: the pair-record table occupies GameState padding at 0x670; it accesses twenty six-byte records but clears 0x80 bytes. Recover a correctly sized shared field and preserve per-entry global-pointer reloads.
- 0807462C–08076034: registry/reset/motion views need a common actor definition for route, probe, callback and collider storage. No source edits were retained.
- Reviewed but not consolidated: terrain transitions 08076910–080772C0, attachment responses 0807C11C–0807E334, terrain targets/effects 0807E334–0807E9D0, visibility/stored motion 08084280–08084F44, approach 080C0668–080C12FC, and child group 080F33BC–080F421C. These still contain overlapping views or untyped context storage. This review was not a proof that they cannot match.
- Resume with deferred ranges in ROM order. The remaining queue from actor recoil 08116280–08116870 onward is unfinished; recoil received an initial source inspection only. No new item was started after the stop instruction.

No whole-ROM build, generated assembly changes, compiler overrides, pins or barriers were added.
