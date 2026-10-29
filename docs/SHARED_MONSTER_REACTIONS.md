# Shared monster pursuit and contact operations

Tags: #matching #scene-actor #golem #skeleton #ghoul #mummy #regalloc #080C9C04 #080C12FC #0808B3EC #080DA97C #080D2584 #08078EE8

The matched Golem, Skeleton, Ghoul, and both Mummy response functions use one
internal chase implementation. The central `SceneActor_UpdateStatus` also reuses
its component records, flag operations, control/state transitions, and terrain
lookup. This is shared C functionality: arrival, random turning, saved control,
route retries, contact animation, and player selection live once. Each monster
retains its response priorities, sounds, timers, and distinct body fields.

## Ownership and verified interfaces

`scene_actor_chase_reaction.h` is the entry header. Its three implementation
headers contain 17 route, player, terrain, tracking, flag, and control records
and 61 named inline operations. Actors use the existing
`SceneActorRegistryResetView`; no new full actor record is duplicated.

Twelve small interface selectors preserve the forms used by verified consumers.
They select ordinary C value domains, field ownership, coordinate outputs,
saved-control reads, and route-failure structure. They do not select registers,
instruction counts, hash buckets, assembly, or compiler overrides. Defaults are
explicit, and the entry header undefines the selectors after inclusion.

| Consumer | State / map-width accessor | Group / coordinate input | Distinct interface choices |
| --- | --- | --- | --- |
| Golem hit reaction | word / stored halfword | generic actor / scalar axes | joined failure status; fraction accessor |
| Skeleton contact | byte / stored halfword | generic actor / stored position | separate failure exits; direct fractions |
| Ghoul contact | byte / stored halfword | generic actor / scalar axes | separate failure exits; direct fractions |
| Mummy movement | byte / arithmetic word | typed actor / stored position | direct route fields; direct route count |
| Mummy recovery | byte / arithmetic word | generic actor / stored position | promoted coordinate outputs; direct saved control; flag components; byte route count |
| Actor status update | word / not queried | local phase views / scalar axes | shared state/control and terrain; generic actor motion query with stored-halfword mask |

The conditionally selected declarations share operation bodies. The two Mummy
functions also share `SceneActorChase_ChooseRoute`. The dispatcher keeps its own
phase orchestration while using the same operations. These interfaces are
measured matching choices, not proof of the original authors' exact boundaries.

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
