# Canonical scene actor components

Tags: #ram #structs #monster #matching

The actor record in `include/scene_actor_reset.h` now embeds the shared body,
attack arrays, targeting state, and saved control state declared in
`include/scene_actor_components.h`. Existing consumers use those members directly.
This replaces misleading flat aliases such as `defaultFlags2CC` at offset `0x118`
with `saved.behaviorFlags`; it does not add a second actor prefix.

The migration used type-resolved GNU C syntax trees. The parser resolved each
member's owner to `SceneActorRegistryResetView` before changing it. It parsed
624 candidate translation units with no parse failures or unresolved relevant
member types. The initial rewrite covered 1,730 distinct member accesses in
404 C files and three helper headers. Three macro definitions were reviewed
and updated at their source locations after the parser identified expanded uses.

## Component layout

| Component | Actor offset | Size | Contents |
| --- | ---: | ---: | --- |
| `body` | `0x50` | `0x48` | Body ID, direction, position, motion, current route |
| `attacks` | `0xB4` | `0x28` | Four first, second and parameter halfwords; four flag words |
| `tracking` | `0xDC` | `0x24` | Typed target, destination, heading, flags, distances |
| `saved` | `0x118` | `0x1C` | Default flags, direction, state, control and motion fields |

The existing `SceneActorChaseTarget` declaration moved into the component header.
The existing `SceneActorChaseTracking` declaration moved there and gained the
remaining recovered fields through actor offset `0xFC`. The heading at `0xE9`
and the script parameter at `0xEA` remain separate bytes.

## Exact member migration

All paths below are relative to `SceneActorRegistryResetView`.

| Old member | Canonical member | Actor offset |
| --- | --- | ---: |
| `id` | `body.id` | `0x50` |
| `direction` | `body.direction` | `0x59` |
| `position` | `body.position` | `0x5C` |
| `motion` | `body.motion` | `0x64` |
| `currentRoute` | `body.currentRoute` | `0x78` |
| `fieldB4`, `fieldB6`, `fieldB8` | `attacks.first[0]`, `[1]`, `[2]` | `0xB4`, `0xB6`, `0xB8` |
| `fieldBC`, `fieldBE`, `fieldC0` | `attacks.second[0]`, `[1]`, `[2]` | `0xBC`, `0xBE`, `0xC0` |
| `fieldC4`, `fieldC6`, `fieldC8` | `attacks.parameter[0]`, `[1]`, `[2]` | `0xC4`, `0xC6`, `0xC8` |
| `fieldCC`, `wordD0`, `wordD4` | `attacks.flags[0]`, `[1]`, `[2]` | `0xCC`, `0xD0`, `0xD4` |
| `fieldDC` | `tracking.target` | `0xDC` |
| `destinationE0` | `tracking.destination` | `0xE0` |
| `unknownE8` | `tracking.unknownC` | `0xE8` |
| `fieldE9` | `tracking.heading` | `0xE9` |
| `unknownEA` | `tracking.fieldEA` | `0xEA` |
| `flagsEB` | `tracking.flags` | `0xEB` |
| `distanceF0` | `tracking.distance` | `0xF0` |
| `rangeF4` | `tracking.range` | `0xF4` |
| `distanceLimitF8` | `tracking.limit` | `0xF8` |
| `defaultFlags2CC` | `saved.behaviorFlags` | `0x118` |
| `defaultFlags2D0` | `saved.statusFlags` | `0x11C` |
| `direction120` | `saved.direction` | `0x120` |
| `defaultState` | `saved.state` | `0x121` |
| `defaultPhase` | `saved.phase` | `0x122` |
| `defaultBehavior` | `saved.behavior` | `0x123` |
| `defaultControl` | `saved.control` | `0x124` |
| `defaultField2B5` | `saved.kind` | `0x128` |
| `defaultField2EA` | `saved.heightOffset` | `0x12C` |
| `defaultFlags2C0` | `saved.motionFlags` | `0x12E` |
| `flags130` | `saved.privateFlags` | `0x130` |
| `update270` | `contactQuery.opaque` | `0x270` |

The attack arrays expose their fourth entries in previously unnamed storage at
`0xBA`, `0xC2`, `0xCA`, and `0xD8`. Tracking also names the stored coordinate pair
at `0xEC`/`0xEE` and the final distance word at `0xFC`. No offsets move.

The target member changes from an integer address to a pointer. Existing
`target + 12` expressions therefore become `&target->position`; leaving the
arithmetic unchanged would scale the offset by the record size. Two old accesses
to a target-relative field at `0x1C` retain explicit byte-pointer arithmetic until
that larger owner record is recovered.

## Recovered storage and callback contracts

Previously unnamed storage now has members for initializer fields at `0x9A`,
`0x9E` through `0xA4`, `0xAC`, `0xB2`, `0x1E2`, `0x1EC`/`0x1F0`, `0x220`,
`0x238`/`0x23C`/`0x240`, `0x24C`/`0x250`, `0x258` through `0x278`, `0x2B7`,
`0x2E8`, and `0x2FA`. Existing signed health and attachment fields keep their
original types; callers can query their raw halfword storage explicitly.

The command table, animation query, and contact query slots are named unions.
The animation slot has the established halfword and word return contracts; the
contact slot has a byte predicate contract. Existing opaque dispatch members
remain available. These contracts represent actual indirect-call domains, not
per-species preprocessor switches.

## Verification

Stock agbcc compile-time layout assertions checked every component size and the
actor offsets of the body, attacks, tracking, saved control, collider, callback
slots, region, context, group, route, and animation state.

The production translation-unit sweep checked all 624 existing candidate units
and the 15 completed initializers present at this integration point. All 639
units matched their ROM bytes. The seven initial compile errors were the old
integer-style target arithmetic described above; after conversion to the typed
position member, all seven matched without any instruction changes. The full
ROM build is performed by the batch integrator.

This commit changes the canonical record and its existing consumers. Migrating
the newly matched initializer copies to that record and sharing their component
operations is the following change.
