# Shared monster pursuit and contact operations

Tags: #matching #scene-actor #golem #skeleton #ghoul #mummy #regalloc #080C9C04 #080C12FC #0808B3EC #080DA97C

The matched Golem, Skeleton, Ghoul, and Mummy response functions now use one
internal chase implementation. This is shared C functionality, including the
arrival state machine, terrain lookup, random turn, saved control packet,
route retry, contact animation, and player selection. The monster files retain
their response priorities, sounds, timers, and distinct body fields.

## Ownership

`scene_actor_chase_reaction.h` is the entry header. The three implementation
headers keep 15 route, player, terrain, tracking, and control records and 52
named inline operations in one place. The actor itself remains the existing
`SceneActorRegistryResetView`; there is no new duplicate actor record.

The seven small interface selectors describe the forms used by verified
consumers. They select ordinary C parameter/result types, coordinate ownership,
and the route-failure join. They do not select registers, instruction counts,
hash buckets, assembly, or compiler overrides.

| Consumer | State / map-width domain | Group / coordinate owner | Route result |
| --- | --- | --- | --- |
| Golem hit reaction | word / stored halfword | generic actor / scalar axes | joined failure status; fraction accessor |
| Skeleton contact | byte / stored halfword | generic actor / stored position | separate failure exits; direct fractions |
| Ghoul contact | byte / stored halfword | generic actor / scalar axes | separate failure exits; direct fractions |
| Mummy movement response | byte / arithmetic word | typed actor / stored position | separate failure exits; direct route fields |

The conditionally selected declarations share their bodies. The contact and
terrain state machines are no longer copied under monster prefixes. These
interfaces are measured matching choices; matching does not establish the
original authors' helper names or prove that these were their exact boundaries.

## What remains specialized

The typed control/state receiver wrappers and the raw versus masked motion-flag
readers remain common interfaces. The one-pointer random-table view also remains,
with one shared definition. Removing that view from the initial Mummy port changed
the generated size and register allocation. A direct masked motion read changed
instruction structure too. Those controls do not prove that every cleaner design
is impossible; they show why deleting the interfaces was not accepted as cleanup.

Sharing the fields and operations does not justify routing every monster through
one giant response function. The distinct outer branches have observable callback
order, return values, animation gates, and timers. Vanargand's palette initializer
belongs to a separate palette/motion family. The behavior dispatcher requires its
own state and component reconstruction.

## Two compiler lessons

The address-PRE hash rule is documented in `tools/scope/PRE_STACK_ORDER.md`.
Use it only after validating instruction shape and bijective stack identities.
Byte distance alone is especially misleading for a permutation of correct slots.

Ghoul and Mummy add a separate lesson: **tail sharing happens after allocation**.
Writing each terminating response at its own branch lets GCC merge identical
tails in `jump2`. Replacing those source-local responses with an early shared
`goto` removes reload choices before allocation is complete. The final control
flow can look the same while a few registers differ. Recover the real response
boundaries before trying to solve the stack order.

## Verification

After extraction, the entire 12,492-byte Golem translation unit and the 6,372-byte
Skeleton unit still compare exactly with the ROM. Ghoul's 6,236-byte and Mummy's
6,316-byte units also match through these headers. The two new functions use no
register pins or assembly hints. Their individual notes record stock-compiler
hashes; the batch integration records the final combined ROM check.
