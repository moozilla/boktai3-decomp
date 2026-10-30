# Shared monster pursuit and contact operations

Tags: #matching #scene-actor #golem #skeleton #ghoul #mummy #regalloc #080C9C04 #080C12FC #0808B3EC #080DA97C #080D2584 #08078EE8

The matched Golem, Skeleton, Ghoul, and both Mummy response functions use one
internal chase implementation. The central `SceneActor_UpdateStatus` also reuses
its component records, flag operations, control/state transitions, and terrain
lookup. This is shared C functionality: arrival, random turning, saved control,
route retries, contact animation, and player selection live once. Each monster
retains its response priorities, sounds, timers, and distinct body fields.

## Ownership and verified interfaces

`scene_actor_chase_reaction.h` is the entry header. Its implementation headers
share route, player, terrain, tracking, flag, and control operations. Actors use
`SceneActorRegistryResetView` and the canonical body, targeting, and group
components. Group and tracking accesses now use `&actor->group` and
`&actor->tracking`, without casting the first member into a different record.

Two former selectors are removed. Mummy recovery explicitly calls
`SceneActorChase_QueryMotionFlagComponent` and
`SceneActorChase_ClearMotionFlagComponent`; other consumers use the actor-field
operations. The component operations reuse the existing two-byte flag record.
This distinction is visible at the call site and no longer configures a header.

Ten interface selectors remain. They are matching devices and unfinished
cleanup. Only Golem and the two Mummy consumers configure them. Skeleton,
Ghoul, and `SceneActor_UpdateStatus` include the shared header with its defaults.
The entry header undefines the remaining selectors after inclusion.

| Consumer | State / map-width accessor | Group / coordinate input | Distinct interface choices |
| --- | --- | --- | --- |
| Golem hit reaction | word / stored halfword | generic actor / scalar axes | joined failure status; fraction accessor |
| Skeleton contact | byte / stored halfword | generic actor / stored position | separate failure exits; direct fractions |
| Ghoul contact | byte / stored halfword | generic actor / stored position | separate failure exits; direct fractions |
| Mummy movement | byte / arithmetic word | typed actor / stored position | direct route fields; direct route count |
| Mummy recovery | byte / arithmetic word | generic actor / stored position | promoted coordinate outputs; direct saved control; flag components; byte route count |
| Actor status update | byte / not queried | local phase views / stored position | shared state/control and terrain; explicit flag component query |

The conditionally selected declarations share operation bodies. The two Mummy
functions also share `SceneActorChase_ChooseRoute`. The dispatcher keeps its own
phase orchestration while using the same operations.

### Remaining selectors and compiler effects

| Selector suffix (`SCENE_ACTOR_CHASE_…`) | Interface difference still preserved |
| --- | --- |
| `TYPED_GROUP` | Typed actor formal versus an opaque object formal plus a typed local. The extra receiver pseudo changes the order of PRE field addresses. |
| `WIDE_MAP_WIDTH` | `s32` arithmetic result versus an `s16` stored-width result. The result mode changes early conversions; the wide form also changes Golem's final instruction structure. |
| `WIDE_STATE` | Word versus byte state parameters. The field stores remain bytes; argument narrowing changes early pseudos and PRE order. |
| `FRACTION_ACCESSOR` | A byte-returning coordinate accessor versus a direct byte conversion. The accessor adds argument and result lifetimes before combining. |
| `SCALAR_CELLS` | Promoted grid axes versus a stored-position receiver. The caller's address and axis temporaries differ. |
| `JOIN_ROUTE_FAILURES` | One returned status versus two failure exits. This changes live ranges before allocation; forcing separate exits in Golem also exchanges its `sb` and `sl` roles. |
| `DIRECT_ROUTE_FIELDS` | Direct field addresses versus address getters. The getter receiver pseudos affect PRE ordering. |
| `DIRECT_SAVED_CONTROL` | Typed packet stores from actor fields versus an opaque packet receiver and scalar getters. Both packet ownership and field-value lifetimes differ. |
| `SCALAR_COORDINATE_OUTPUT` | Scalar coordinate calculation and typed slot writes versus narrowed member assignments. These expand through different store paths before CSE. |
| `ROUTE_COUNT_ACCESSOR` | A byte-returning count query versus direct member access. The query introduces a receiver and a narrowed result before `Mod`. |

The cleanup checked each switch with fresh complete-unit builds, then checked
the existing forms together. No one common form preserved all six units. With
all defaults, Mummy movement had the same physical instructions after matching
its 45 spilled address identities, but no odd PRE table size could reproduce
their required relative order. Golem's default-form experiment likewise had
no table-size solution for its 61 addresses and also changed high-register
allocation. These are constraints on these particular source forms, not proof
that a different shared operation cannot match.

The retained settings were also checked by removing every subset of the
existing defines in each of the three configured consumers. None of those
subsets preserved the whole function. Further removal needs a better shared
operation or a new exact reconstruction; renaming the settings would not
complete this cleanup.

`SceneActorChaseFlags16` explicitly has two-byte alignment and extent. Its real
consumers include actor flags at both `+0x2D4` and `+0x2D6`; the latter cannot
assume the default four-byte aggregate alignment of this AGBCC ABI. Target
layout assertions and all affected exact compiles cover that declaration.

## What remains specialized

Typed and generic control/state receivers and raw versus masked motion queries
remain common interfaces. The one-pointer random-table view also remains, with
one shared definition. Removing that view from the initial Mummy port changed
size and allocation. A direct masked motion read changed instruction structure.
Those controls do not prove every cleaner design impossible; they explain why
those deletions were not accepted as cleanup.

The status updater retains raw-mask accessors and normalized word predicates.
The latter share one Boolean implementation. It also has a real two-property
environment snapshot: mode and cached solar level both have consumers. These
source boundaries explain the observed compilation without claiming historical
uniqueness. Some signed interpretations of canonical raw storage remain local
views. The one-halfword palette countdown view is explicitly two-byte aligned.

Sharing operations does not justify one universal monster response function.
Outer branches retain observable callback order, return values, animation gates,
and delays. Vanargand's initializer belongs to a different palette/motion family;
its new `motion_track.h` is shared with the existing motion initializer.

## Compiler lessons for other workers

Use the address-PRE model in `tools/scope/PRE_STACK_ORDER.md` only after validating
physical instructions, resolved branches/literals, and bijective stack identities.
A lower byte distance may be a worse source starting point. The status updater's
seven-byte candidate could not satisfy the required order at any odd table size;
a 35-byte candidate had the right relative identities. Measured predicate
interfaces then produced an exact composition. Shared cleanup remained exact.

Ghoul and Mummy also show that **tail sharing happens after allocation**. Writing
each terminating response at its own branch lets GCC merge identical tails in
`jump2`. An early shared `goto` removes reload choices before allocation ends.
Recover response boundaries before solving stack order.

The reusable halfword-member flag operation first resolved Mummy's narrow-value
lifetimes, then resolved the status updater's final physical-register ties.
That is a demonstrated transfer between two difficult functions. The result does
not establish a mechanical solution for every remaining monster draft.

## Verification

The full 12,492-byte Golem unit, 6,372-byte Skeleton unit, 6,236-byte Ghoul unit,
6,316-byte Mummy movement unit, and 4,852-byte Mummy recovery unit remain exact.
The 5,956-byte status updater matches through the same headers. The new targets
have no pins, assembly hints, or compiler overrides. Per-function notes record
stock and complete-unit checks; batch integration records the combined ROM gate.
The selector and canonical-member cleanup was checked again after the shared
group migration: all six complete units, 42,224 bytes, have zero differing bytes.
