# Shared monster initialization groups

Tags: matching, structs, compiler, scene-actor, Skeleton_Init, Golem_Init, Mummy_Init, Vorn_Init

The four grouped initializers now use the canonical actor, body, attack,
targeting, saved-control, collider, pursuit-group, gate, range, and route
records. Species-specific views contain only the private tail after offset
0x624. Their common initialization operations live in
`scene_actor_init_actor.h` and `scene_actor_init_group.h`.

## Shared operations

`scene_actor_init_group.h` supplies one implementation of each targeting
query/mutation, optional targeting parser, range parser, gate initializer,
route accessor, route-position calculation, and optional route parser.
The group constructor is also shared. Its active mode is a byte argument;
Mummy supplies 3 and the other three species supply 1.

The constructor has two real entry points. `MonsterInit_InitializeGroup`
installs the group fields and callbacks. `MonsterInit_InitializeGroupWithBounds`
calls it and then translates the initial bounds using the body's position.
Octopus, Ivy, and Axe omit that translation in the ROM, while the four units
in this change perform it. An optional-body guard was tested and rejected:
this compiler retained the test for a pointer derived from an actor, adding
instructions. The explicit wrapper preserves the operation sequence and
shares the complete field initialization body.

The header declares the common reaction callbacks using their existing
function definitions. In particular, the frame-taking callbacks have their
second argument, and `sub_0807F130` has its signed result. Old no-argument
forward declarations were removed from these initializers.

The sequence-selector callback constructor is shared separately in
`scene_actor_init_dispatch.h`. Its result, sequence, action, contact predicate,
and default-reaction parameters have their actual callback signatures. It
uses the shared typed writes and the decoded `SceneActorCommandHandler` table
at `gUnk_086108B0`. Its default sink handler is declared with its existing
actor/frame signature. This removes the four identical primary callback
installation bodies without introducing a runtime descriptor object.

## Integer domains and compiler evidence

Targeting flags and range-radius queries return word values for bitmask and
arithmetic operations. Gate duration and the stored range parameter use
halfword interfaces. Route IDs use the shared optional byte reader. The
source operations and runtime instructions are the same across all four;
there are no species selector macros or duplicated getter bodies.

Before the remaining stored keyword readers were typed, the shared group
header left only PRE stack-slot permutations in Golem and Vorn. The read-only
hash model verified every modeled spilled address. Golem required an early
RTL instruction count of 1580–1583 rather than 1574; Vorn required 1684–1687
rather than 1682. The existing `MonsterInit_ReadKeywordHalfword` interface
was then applied to Golem's stored M and K values and Vorn's stored M value.
Those storage contracts restored the required counts and exact stack order
without changing the physical instruction sequence.

The common actor extraction preserves the original motion constructor:
`MonsterInit_SetMotionControl(actor, 0, 0)` initializes the control fields and
its enabled flag, then the initializer disables that flag. The runtime setter
in `sub_080B01A4.c` establishes that constructor behavior. Flattening this to
five zero stores changes the early lifetime of the constant one and fails
the grouped initializers' instruction match.

The optional four-entry attack loop is shared through
`MonsterInit_ReadAttacks`. Its halfword readers are the same ones used to
recover the compiler's original induction-variable choices. Calling this
operation from the grouped body initializer leaves every byte unchanged.

## Verification

All four complete translation units were compiled by the unmodified stock
compiler and compared with the ROM after actor, group, attack-loop, and
motion-operation extraction:

| Function | Address | Exact bytes |
| --- | --- | ---: |
| Skeleton_Init | 080C3260 | 4684 |
| Golem_Init | 080CFC14 | 4072 |
| Mummy_Init | 080DCC98 | 4188 |
| Vorn_Init | 080B43BC | 4332 |

Each reports zero differing bytes. These source files contain no inline
assembly, register pins, compiler knobs, selector macros, or extra stores
added to perturb allocation. Full combined ROM validation remains the batch
integrator's final gate.
