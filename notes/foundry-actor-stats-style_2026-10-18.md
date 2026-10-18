# Actor stats, preview palette and style foundry pass

Tags: #matching #foundry #actor-stats #scene-actor

Worked through 0811C5F8, 0811C67C, 0811C89C, 0811D0C4, 0811D384 and
0811D4AC in that order. Five exact candidates remain in `wip/foundry/`;
0811C89C remains fuzzy at 88.0% after five checks, with saved-register and
allocator-address scheduling differences. Per-function records are in
`build/foundry/FUNC/result.txt`. No production sources were integrated and
no commits or combined build were made.

0811C5F8 applies a pending clamped increment; 0811D0C4 applies modifier-adjusted
pending damage and sets a depleted flag. Their shared tentative ActorStats
view and 0811D4AC's scaled parameter fields are in `include/actor_stats.h`.
These stat-bearing objects must not be confused with scene actors solely
because some offsets coincide. 0811D4AC's modifier callee 0811E970 filters
kind 7, as established by its retained neighboring C.

0811C67C selects a palette/resource index from either link-player element
values or equipped party categories. The two link slot views begin at 0x120
with stride 0x40; their element field is at +0x22. This overlaps existing
session/exchange views, so the shared header adds a separate layout without
moving or replacing established members. Its boolean validator needs explicit
labels to preserve the materialized signed range test.

0811C89C initializes a preview's flash/normal palettes, assigns palette and
tile references to five 0x60-byte display parts, then advances and wraps a
tile counter. The shared modifier header now exposes that preview model and
palette controller. The allocator identity remains provisional.

0811D384 initializes a tentative SceneActorStyle from game-state fields,
counts entries with 0811D334, and snapshots the halfword at game-state +0x818.
The shared game-state header replaces two unknown bytes at +0x7FE with the
observed halfword; all established offsets are preserved.

Header checks passed for area_effect_link.h (3 existing matched consumers),
modifier_list.h (44), and game_state.h (17). New actor_stats.h and
scene_actor_style.h have no production consumers yet. The earlier exact
ActorStats candidates were explicitly rechecked after completing their layout.
All candidates use plain C without register pins or instruction bodies.
