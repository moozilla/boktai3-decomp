# Remaining monster state and type recovery

Tags: #monster #types #matching #phase3

The first six objects of the remaining-monster pass preserved every exact
match. Stock unit checks verified each changed C file. Header checks also
covered all 18 dependents of `ghoul.h` and both dependents of `ivy_phase.h`.

| Object | Printed warning lines | Weighted steering uses |
|---|---:|---:|
| Ghost | 102 → 98 | 108 → 101 |
| Lun | 73 → 56 | 85 → 61 |
| Spider | 64 → 62 | 63 → 61 |
| Ivy | 62 → 58 | 69 → 57 |
| Ghoul | 61 → 56 | 61 → 56 |
| Octopus | 56 → 45 | 65 → 47 |

## State transitions and native fields

- Ghost embeds the shared playback cursor and reads the incoming collider's
  flag field. Route selection (08097CB4) and deceleration (0809679C) use the
  shared chase state operations. Visibility uses the actual timer and sprite
  flag fields.
- Lun's route slot carries both the registry and playback views of one cursor.
  The participant, hover, and terrain callbacks use the existing actor and Lun
  records. The linked idle callback (0809C834) matches with
  `SceneActorTerrain_EnterState`. The similar chase helper moved the zero
  before the first field address. In lift recovery (0809B7FC), the resumed
  state consumes the existing initializer. A plain first reaction-flag write
  still moves the 0x10 immediate across the address literal.
- Spider's windup captures a typed target, and its probe and pursuit callbacks
  use the native chase group. The route selector (080A48E0) takes the same
  narrow cursor as Ghost and Lun. The larger waypoint walk (080A4798) still
  needs its existing index and cursor allocation.
- The Ivy blend and neutral callbacks install each other on the same child.
  Their merged type also covers bobbing and delayed orbit. Five shorter
  phase-object units use the larger shared record field by field. The
  automated merger refused those larger extents and required joint scope
  review for two declarations in the same child unit.
- Ghoul (0808A90C, 0808CC48) uses the reviewed tracking check and common route
  retry and delayed-response operations. Its constructor initializes the
  native chase probe. The two collider setups reuse a shared packed vector.
- Octopus uses the terrain state operation for entry and idle state, and the
  existing delayed response and drawable operations for capture release
  (080EA65C). Its one-iteration player scan (080EC024) still has a real loop in
  the ROM. The shared target view matches with that loop in place.

## Remaining compiler decisions

The Ghost forwarder (080956D4) needs the shifted 0x800 mask before the
collider halfword load. The direct expression reverses them. Ghost's afterimage
flip comparisons (080980FC) still differ with direct masks, bit-fields, and XOR.

Spider wander (080A4018) still has two proved-zero sites. Linked sample indices,
pointer indexing, and a single cursor written with modulo all miss the required
joint allocation of the body, table, and cursor. An earlier compiler diagnostic isolates those three assignments.
[Hard sites note](hard-sites_2026-11-23.md#2026-11-26-spider-wander-allocation-without-equivalence-control)

The Ivy defeat routine (080EE1B0) changes reload rotation with a direct random
table. The child group's seven entry callbacks change mask setup or register
allocation with either a direct field test or a captured field value.

A direct engine test in the shared Ghoul target helper matched 16 of 17
dependents. The states unit still needs its existing setup. Directing the turn
flags through plain fields changes allocation across the switch (08086400).

The working branch retains matching expressions at those sites. The batch
reports zero linter errors.


## Task and component layouts

The continuation reviewed the rest of the warning queue. Stock checks matched
all 101 changed translation units across the pass. All changed headers passed
their full dependent checks. Code matching remains at 100%, a gain of 0
percentage points. The pass removed 86 printed warning lines and 133 weighted
constant uses with zero linter errors in the owned lane.

- Slime, Mimic, Bat, Crow, and Bee reuse native position, collider, tracking,
  and state transition records. Slime and Crow keep addressable source words
  for their BIOS fixed-source fills. Bee's three blocked-action checks and
  terrain transitions now use the shared operations.
- Sword creation (080F752C), component dispatch (080F63C4), script setup
  (080F7458), and both initialization passes use one size-checked task with
  four embedded 0x194-byte attack components. Attack initialization
  (080F6F90) reuses the native packed vector and sequence playback. The
  damage routine (080F645C) uses typed reaction and bonus fields. With plain
  blocks, the reaction flags outrank the score-counter address and take
  its ROM register. The instrumented allocator confirms that reversing
  their priority reproduces the remaining instructions.
- Enemy particle creation (0820D614) and update (0820D2C4) establish a
  16-slot task with 0xB0-byte slots. The emitter passes its embedded
  `DisplaySequenceState` directly. The pause check and position fill use the
  existing engine and SDK operations. Direct animation addressing in the
  updater changes CSE and the competing parameter and velocity registers.
- Plasma Cannon (0820F378), Garmr Statue (08208740), Poison Hand (0820E2A4),
  Statue (08203DF4), and Screamer setup (0820CD00) use existing frame or
  playback operations. With recovered records, Plasma and Dragon Puppy
  initializers (0820F830 and 0820B7E0) accept their direct script defaults.
  Wind Cannon's collider setup (082093F8) matches with native vector
  component stores. Vulcan targeting and damage (08206C00 and 082074F0)
  match with direct expressions at two facing tests and four explosion
  calls. The final use in each routine still affects reload allocation.
- Ooze's script reader (08205A6C) caps the delay table at eight entries.
  Its dispatcher (08206030) and attack callback (08205B70) increment
  different timers. Popbud and Pollen Flower now share their task layouts
  across setup and the callbacks that passed the stock checks.

The direct player-slot gate repeatedly materializes bit 4 before the pause
branch. The ROM materializes it after that branch and before loading the
slot word. The remaining gates use the matching expression with the
symbolic player-slot status. A second pass tried each draw-limit and jitter
mask separately across nine hit-spark handlers. Each isolated fold still
changed the ROM sequence, so those expressions remain in place.

The coordinator validates the combined ROM during batch integration.


## Resumed refinement

The follow-up retained two direct field operations. Ghost's visibility
transition (0809852C) reads the timer directly, and Spider's wander routine
(080A4018) decrements its timer in place. Folding one retreat scale in
Ghost (08096D38) and two individual reads in Lun (0809AE0C and 0809D2A8)
removes three weighted constant uses. Folding the second Lun terrain clear
still changes the zero pseudo's allocation. Spider's constructor (080A4AFC)
keeps the addressable word consumed by the BIOS fixed-source fill in its
widget and pursuit initialization. Separate and combined SDK wrapper
substitutions change code generation. The linter annotation records that
source operand. All three changed units passed their stock checks.
