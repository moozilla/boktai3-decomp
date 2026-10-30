# Canonical scene actor groups and routes

Tags: #structs #ram #monster #matching

`SceneActorRegistryResetView.group` now embeds the existing shared
`SceneActorChaseGroup` at actor offset `0x3D4`. The group is `0x250` bytes and ends
at `0x624`, where the pre-existing animation storage still begins. The old flat
actor members in that range are replaced by paths into this group.

`include/scene_actor_group.h` owns the records. The full pursuit group contains
one `SceneActorRegistryControlGroup common` prefix, followed by the route,
saved control, motion and callback fields. Other species retain their private
tail records. The existing common prefix is enriched, not copied into a second
header.

## Checked layout

| Record/member | Size or actor offset |
| --- | ---: |
| Full group | size `0x250`, actor `0x3D4` |
| Common group header | size `0x74` |
| Gate | size `0xC`, actor `0x414` |
| Control/range | size `0x1C`, actor `0x420` |
| Home route | size `0x14`, actor `0x448` |
| Saved control | actor `0x460` |
| Motion | actor `0x5B8` |
| Route owner / destination / turn | actor `0x5D0` / `0x5D4` / `0x5D8` |
| Interaction / response callback | actor `0x618` / `0x61C` |
| Following animation storage | actor `0x624` |

The old `SceneActorChaseRouteLink` and `SceneActorRegistryRoute` now use the one
registry route record, with a typed point, current-point and branch state, and
script route/point identifiers. The gate and control records also expose the
initializer fields previously hidden in padding. Signed gate flags and the
existing signed turn count retain the chase record's domains.

## Member migration

The migration resolves the owner of each C member through a GNU C syntax tree.
It analyzed 626 candidate translation units with zero parse errors or unresolved
relevant owners. It changed 545 distinct references in 171 files, including
three shared chase headers. One macro-expanded coordinate was resolved at its
actual member token. Seven global offset assertions in the status updater were
migrated explicitly and still pass.

The following paths are relative to `SceneActorRegistryResetView`:

| Previous member | Canonical path |
| --- | --- |
| `group` | `group.common` |
| `route` | `group.homeRoute` |
| `contactNode` | `group.contactNode` |
| `word460` | `group.saved.control` |
| `field464` | `group.saved.state` |
| `field465` | `group.saved.phase` |
| `field466` | `group.saved.behavior` |
| `field467` | `group.saved.kind` |
| `field468` | `group.field468` |
| `delay46C` | `group.delay46C` |
| `callback470` | `group.callback470` |
| `attachment` | `group.attachment` |
| `motion5B8` | `group.motion5B8` |
| `field5C0` | `group.field5C0` |
| `field5C2` | `group.field5C2` |
| `field5C4` | `group.field5C4` |
| `field5C6` | `group.field5C6` |
| `countdown5C8` | `group.countdown5C8` |
| `field5CA` | `group.hitReactionLatch` |
| `unknown5CB` | `group.unknown5CB` |
| `effect5CC` | `group.effect` |
| `routeObject` | `group.route` |
| `state5D4` | `group.destination` |
| `field5D8` | `group.turn.direction` |
| `field5D9` | `group.turn.savedDirection` |
| `field5DA` | `group.turn.turnsRemaining` |
| `behaviorControl` | `group.contactControl` |
| `routePhaseCallback5F4` | `group.routePhaseCallback` |
| `motionFailureCallback` | `group.fallback` |
| `interactionCallback` | `group.interactionCallback` |
| `responseCallback` | `group.responseCallback` |

The common prefix's `flags414`, `field416`, `field418` and `entryControl41C`
become `gate.flags`, `gate.duration`, `gate.counter` and `gate.control`.
The chase group's placement, gate and control members now go through `common`.
Registry route `fieldC`, `parameterA` and `parameterC` become `currentPoint`,
`routeId` and `pointId`.

## Verification

Stock agbcc layout assertions check all record sizes and the embedded offsets
above. All 641 production-unit checks pass with zero differing bytes, covering the
existing consumers plus all 17 exact initializers. The batch integrator runs the complete ROM build after combining
this migration with the remaining initializer helper extraction.
