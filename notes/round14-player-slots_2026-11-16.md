# Round 14 checkpoint 4: player-slot ownership

Tags: phase3, checkpoint4, shared-types, player-slots, global-ownership

Source-evidence base: `c940902f6ba266133e4e3cd5edf46f308cd03136`.
Accepted source at integration: `eee8e20b6abe7d3a8fc433d8e73f71c8e549b665`.
Worker source head: `eee8e20b6abe7d3a8fc433d8e73f71c8e549b665`.

The shared consumers now use the existing `struct Player *gPlayers[4]`
declaration in `include/player.h`. Twenty-four source units lose their legacy
name completely; Octopus's capture request also moves to the canonical array.
Five chase/status headers use the same owner. Thirteen of fifteen conflicting
private declarations are removed, and twenty-five known status-flag accesses
use `Player.statusFlags` directly instead of a private player view.

**The ownership cleanup is not complete.** Two legacy declarations remain in
`SceneActorTarget_FindNearbyPlayer` and `RouteSpriteActor_PlanWanderPath`.
Ivy's related nearby-player query also retains a literal-address read. Their
natural canonical drafts are preserved rather than replaced by a new alias,
cast workaround, weighting loop, or compiler override. No Player layout was
changed, and this series makes no claim about a net diagnostic reduction.

All 54 affected complete source units match the ROM with stock agbcc:
**515,476 bytes, zero differences.** The coordinator owns the combined build.

## Actual RAM object and lifetime

The canonical RAM row is `symbols/ram.csv`'s object at `02000580`, size `10`
(hexadecimal), with `extern struct Player *gPlayers[4];` owned by `player.h`.
The next object, `gPlayerEffectPendingCount`, starts at `02000590`.
The reset and four-slot consumers independently corroborate the array extent.

| Slot | Address | Stored value |
|---:|---:|---|
| 0 | `0x02000580` | Player actor pointer |
| 1 | `0x02000584` | Player actor pointer |
| 2 | `0x02000588` | Player actor pointer |
| 3 | `0x0200058C` | Player actor pointer |

`tools/build.py` resolves address-suffixed RAM names from their suffix and named
objects from `symbols/ram.csv`. The existing linked script assigns both
`gUnknown_02000580` and `gPlayers` to `0x02000580`. A scalar declaration of the
former therefore reads slot zero; it does not identify a separate singleton.
No declarations or direct references to separate `02000584`, `02000588`, or
`0200058C` aliases were found in `src/` or `include/`.

| Lifecycle operation | Existing source evidence |
|---|---|
| Reset all slots | `Player_ResetLinkState` (`src/fn/sub_08158C14.c`) clears four `gPlayers`, tracked-ID, and slot-status entries, then clears the player count and related state. |
| Publish regular player | `Player_InitializeFromScript` (`src/fn/sub_0815F2C8.c`) initializes components, publishes the actor by its body-mode slot byte, and increments `gPlayerCount`. |
| Remove regular player | `Player_Destroy` (`src/fn/sub_0815F224.c`) removes the actor's components, clears that slot, and decrements the count. |
| Publish link player | `Player_OnCreate` (`src/fn/sub_08243A50.c`) initializes the link-player components, publishes the actor, and increments the count. |
| Remove link player | `Player_OnDestroy` (`src/fn/sub_082439EC.c`) unregisters its components, clears the slot indexed by the word at actor `+0x18`, and decrements the count. |

The apparent slot-offset disagreement is explained by the body initializer.
`Player_InitSlotFromScript` in `src/player_3.c` and
`LinkPlayer_ReadSlotFromScript` in `src/fn/sub_0823AC78.c` store script option
`i`, defaulting to zero, at actor `+0x18`. Both
`Player_InitBodyFromScript` (`src/fn/sub_0813FFE4.c`) and
`KeyframeActor_InitBodyFromScript` (`src/keyframe_actor.c`) pass its low byte to
`SceneBody_Init`. That function (`src/fn/sub_08225AB4.c`) stores the value at
body `+8`, hence player `+0x2C`. The normal four-slot domain makes these indices
consistent. The provisional Player field name `movementRequestBlocked` at
`+0x2C` was not renamed during this ownership pass.

Slot zero is the default script slot and the fixed slot used by many gameplay
consumers. The link-player creation code separately compares the actor's slot
with the session's local-player index; the cleanup does not infer that every
link session's local player is necessarily slot zero.

The array is also traversed as an array outside these lifecycle functions:
`ActorPool_RefreshTargets` (`src/actor_pool.c`) copies four targets, while
`SceneActorAttachment_RetargetNearestPlayer` (`src/fn/sub_0807D220.c`) walks
`gPlayerCount` entries and skips null actors. This pass preserves each caller's
existing iteration bound; it introduces neither a new clamp nor a new bound.

## Inventory and migration scope

Before this series, the legacy name appeared on 104 lines in 26 source units
and five headers. There were nine scalar declarations, four source-local array
declarations, and two header array declarations. Their incompatible views were:

| Previous declaration family | Locations | Accepted treatment |
|---|---|---|
| Scalar `struct Player *` | `lun.c` | Replace every use with `gPlayers[0]`; remove the declaration. |
| Scalar `void *` | `actor_animation_controller.c`, `fn/sub_0801CC24.c`, `fn/sub_080F67AC.c`, `fn/sub_081FAE0C.c`, `fn/sub_0806D438.c` | First four migrate to `gPlayers[0]`; the route-planner helper remains open. |
| Unused scalar `u8 *` | `axe.c`, `spider.c` | Delete the unused declarations. |
| Scalar private `EffectPlayer *` | `timed_effect.c` | Use `gPlayers[0]` and `Player.statusFlags`; remove the private partial player struct. |
| Private player-pointer arrays | `fn/sub_0802592C.c`, `fn/sub_08028F78.c` | Use real `Player *` / `Player **` records, the canonical array, and known Player fields; remove the private player structs. |
| Integer pointer-word array | `fn/sub_081A2C80.c` | Use `Player **` input traversal, actual pointer-valued target slots, and `&player->position`. |
| Byte-pointer arrays | `octopus.c`, `scene_actor_chase_shared.h`, `scene_actor_status.h` | Both shared declarations disappear; Octopus capture uses the canonical indexed slot, while its nearby-player query retains its local alias. |

The direct Player field substitutions include status flags at `+0x20`, action
mode at `+0x418`, facing-change effects at `+0x4F4`, and position at `+0x30`.
These offsets already belong to `player.h`; no new type alias or overlay was
introduced to disguise a disagreement.

The five edited headers are `scene_actor_chase_shared.h`,
`scene_actor_chase_step.h`, `scene_actor_chase_direct_arrival.h`,
`scene_actor_status.h`, and `ghoul_disc.h`. Their transitive dependent counts
are 42, 39, 2, 5, and 2 respectively; their union is 42 complete source units.
The twelve additional directly affected units bring verification to 54.

`player.h` had 685 source dependents before this pass and 710 afterward.
The header's contents and Player layout were not changed. The extra 25
consumers obtain their declaration from the existing owner through direct
includes or the shared chase header. The complete source-name and dependency
lists are in the inventory artifact, rather than inferred from include counts.

A broader textual inventory at the accepted source revision finds direct
named or literal references to this RAM object in 224 source units and nine
headers. Those existing canonical consumers are recorded for future type work;
this pass does not rewrite all of their private object views.

The dog, golem-main, and skeleton-main migrations follow the coordinator's
already integrated defeat-gate/prototype changes. Their four synchronization
commits in the worker branch are not part of this source series. The inventory
lists the 25 unit commits and the one mechanical shared-header commit in order.

## Measured component frontier

A probe compiled with pinned stock ARM agbcc measured the following layout:

| Property | Value |
|---|---:|
| `sizeof(gPlayers)` / pointer stride | `0x10` / `4` |
| Player script slot | `+0x18` |
| Player status flags | `+0x20` |
| Player body prefix | `+0x24` |
| Player mirrored body mode/slot | `+0x2C` |
| Player position | `+0x30` |
| Player movement-source pointer | `+0x40` |
| `SceneActorChasePlayer` flags / target / cell pointer | `+0x20` / `+0x24` / `+0x40` |
| Pointed chase-cell index | `+6` |
| `SceneVec` size / alignment | `8` / `4` |
| `ContactPosition` size / alignment | `8` / `4` |

The two vector types have identical measured size and alignment under this
compiler. They are nevertheless distinct C struct types, so a direct aggregate
assignment between them is not a valid replacement for a typed copy. The
route-planner failures must not be described as an ABI alignment discrepancy.

Existing chase target and movement-source views remain explicit where they
are still needed. The target begins inside the Player body prefix at `+0x24`.
`ActorStatus_FaceTarget` now anchors that existing view at
`&gPlayers[0]->positionalEffectGroup`. That member supplies the known body-prefix
address; its provisional scalar type does not recover the complete target
subobject. This is a declaration-ownership improvement with residual layout
debt, not a claim that the body view has been eliminated.
The pointer at `+0x40` refers to a separate movement-source record; it is not a
license to substitute a fixed field in the Player object. For example, body
initialization installs the record at player `+0x284`, and named-object release
in `src/fn/sub_081029F8.c` restores that self-owned source. A coherent body/probe
component definition is separate work from choosing the owner of `gPlayers`.

## Remaining matching dependencies

### Octopus nearby-player query

`SceneActorTarget_FindNearbyPlayer` (`0x080EC024`, inside `src/octopus.c`)
initially reads slot-zero flags through `gUnknown_02000580`, then traverses
`gPlayers` through its existing union view. Using one canonical object lets the
compiler retain the array address between those regions. The second table
literal load becomes a register copy, the actor's allocation changes, and one
literal/padding pair disappears. The complete unit shrinks from 14,980 to
14,972 bytes.

The historical commit `40764accb06ee8484d5383142dbd8fd442fa31c5` is directly
relevant: it replaces a volatile pointer read from literal `0x02000580` with
the separate legacy array declaration/read while retaining a
`MATCHING_CONCESSION: register-pinning` annotation. Thus this is an inherited
matching workaround with a measured compiler mechanism, not evidence that two
separate runtime objects exist.

A real `Player **` traversal with the known body-prefix address keeps the
14,980-byte extent but still fails. An ordinary indexed single-slot traversal
also fails. No alternate linker alias, artificial volatility, integer handle,
or source-order permutation was adopted. The capture request's independent
indexed access does match and is included in the accepted Octopus change.

### Route planner's source view

`RouteSpriteActor_PlanWanderPath` (`src/fn/sub_0806D438.c`) passes the address of
the legacy scalar slot to an existing generic helper. That helper binds the
loaded object to a private `WanderSource` whose position has type
`ContactPosition`. Canonical Player access instead exposes `SceneVec`.

| Natural candidate | Complete unit | Result |
|---|---:|---|
| Existing production source | 776 bytes | Exact |
| `Player **` source plus four field assignments | 784 bytes | Nonmatching |
| Ordinary eight-byte `memcpy` | 764 bytes | Nonmatching; emits a libc call |
| Whole `ContactPosition` value conversion | 820 bytes | Nonmatching |
| Existing component view, helper takes the source object from `gPlayers[0]` | 776 bytes | Four differing bytes: the slot load and output-address setup exchange order |

The last candidate adds no cast and keeps the existing component view visible,
but it is still not exact. Argument-order permutations were not used to force
those instructions. The private helper appeared in the earlier matching
commit `e7654438669f4215ec19274c9a29f2d7f1c885d6`; that commit also introduced
an unrelated random-table asm barrier. Its annotation alone does not prove
that this particular helper was fabricated solely for allocation. This note
records the observed code-generation dependency and leaves the original
component/API recovery unresolved.

### Ivy's separate literal-address query

`SceneActor_FindNearbyPlayerTarget` in `src/ivy.c` is a useful algorithmic
relative, but it retains a literal `0x02000580` read followed by a named-array
traversal. Replacing the initial literal read with `gPlayers[0]` shrinks its
21,464-byte unit to 21,456 bytes. Also removing the traversal's existing
pointer/integer round-trip produces the same nonmatch. Both drafts are
preserved. The unit's accepted status-flag ownership change is independent;
the query itself remains unchanged. Code-generation dependence alone is not
used as proof of the original intent of every local or view in that function.

## Validation and durable evidence

Validation used stock agbcc revision
`da598c1d918402c42c0c0d7128ba14567f3175e9`, the normal build flags, and the
reference ROM with SHA-1 `2651c5e6875ac60abff734510d152166d211c87c`.
Every affected complete unit was compared with an explicit original text
extent. The 52 canonical draft units that passed remained unchanged; the two
nonmatching draft units were restored to their accepted source forms and
checked independently, producing the final 54/54 result. No whole-tree build
or linter sweep was run by this worker.

- [Inventory, declarations, layout probe, dependency lists, and complete-unit results](evidence/round14-player-slots-inventory_2026-11-16.json)
- [Replayable source candidate patches](evidence/round14-player-slots-candidates_2026-11-16.jsonl)

The candidate file contains three accepted baselines and nine rejected natural
source candidates. Every candidate is a complete patch against the integrated
source revision above and records the resulting full-source SHA-256. All nine
nonempty patches were independently applied in temporary trees and their
resulting source hashes verified. No generated assembly or RTL is included.
Whole-unit byte and aligned-instruction measurements are labeled as such;
when a query changes size, shifted later code and literal data contribute to
those totals.

The remaining source is deliberately visible: two legacy declarations and one
literal-address query, plus the already existing chase body/probe views. The
shared header conflict that prevented Lun's helper reuse is removed without
claiming those remaining source choices have been solved.
