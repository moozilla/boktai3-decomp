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
