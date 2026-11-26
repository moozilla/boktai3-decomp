# Monster route, control, and sprite follow-up

Tags: #monsters #types #phase3 #matching

## Centipede and Armor

CentipedeBody_ApproachWaypoint (080A7700) initializes the oscillation
fields directly and uses SceneActorChase_SetControl for the entry
transition. Retreat and pursuit read the owning body's waypoint and
timers. Centipede_ResetRouteHistory (080AB4E0) assigns all six entries
with an index. The pending impulse handler uses the segment's heading,
collider radius and offset fields. Direct vector writes change the
128-byte impulse routine to 148 bytes. The continuation removes nine
steering uses.

Armor's layered-end and defeat routines read the actor heading and
phase fields and the embedded launch group. The elemental cases in
Armor_InitCombatColliders still need their separate tails. Combining
them shortens the routine from 652 to 528 bytes. The direct collider
stores fail the stock dependent checks.

## Golem and Dog

Golem_AlignChargeTarget (080C47DC) uses its embedded SceneActorBody
and ContactPosition fields. Golem_RestoreRoute (080C4A9C) uses the
chase group's gate, route, destination and fallback. The profile
initializer writes the room actor's profile ID. A direct mask in
Golem_ApplyDuration shortens its caller's unit by eight bytes.

Dog_SelectRetreatDirection (080DEF78) shares the chase route operation.
Dog's timers and idle tracking reset use their owning fields. In the
direct turn-direction trial, the compiler drops the live terrain-map
address and changes reload choices 5 and 46. The address form is still
required for the matching source.

## Axe and Axe Armor

Axe's returning trail and timed palette flicker use the Axe owner.
Keyword O supplies the trail angle decay and returning speed.
Axe_UpdateTrailColliders (080FC998) uses typed slots and collider
entries. The two trail palette updates and secondary collider removal
use indexed loops. In Axe_InitEffectPalettes, spelling the literals
directly changes reload choice 1 and moves the white color load
relative to the black-bank address.

AxeArmor_RecordDefeat (08204748) uses ScriptUtilityArguments and the
AxeArmor fields. The eighth argument comes from the parent pointer at
+0x15C. AxeArmor_ApplyHitReaction (08204990) uses an indexed three-spark
loop and direct spread masks. AxeArmor_InitializeFromScript reads the
sequence commands and terrain rows directly and uses a literal for the
already-dead animation call. The destroy routine still preserves the
sprite remover's return register. Declaring the wrapper void moves the
return address from r1 to r0.

## Totals

The continuation reduces the diagnostic rows from 653 to 646 and the
steering uses from 717 to 705. Centipede accounts for nine removed uses
and Axe Armor for three. The six objects report zero linter errors.
All three dependents of the Axe header pass.
The stock compiler matches all 20 changed units across their recorded ROM extents.

## Third pass

Armor_ApplySceneCommands (0818A500) now reads SceneEventReceiver and
SceneEventCommand fields. Armor_InitFromScript (0818C0BC) clears the nine
fragment callback pairs with an indexed loop. Direct elemental reaction
tests change reload choice 23 in Armor_ReceiveColliderContact (0818B7AC).
Changing the reload choice in scope leaves 17 instruction differences.

Centipede_ShiftSegmentTrail (080A9988) reads the sprite's position
directly. The bridge pass at 080A9DB0 indexes its segment positions.
Direct trail flag expressions shorten the 280-byte routine to 224 bytes
and change the allocation of the loop base.

Golem's charge check reads the tracking target, distance and limit from
the owning actor. The recovery path reads the route probe's destination.
Dog's arrival animation reuses the standard duration operation and
Dog_Init steps the tile drawable using its animation member.

The Axe cooldown tick, auxiliary sprite removal and collider cleanup now
index their arrays. The direct sprite-expiry loop keeps a single slot
cursor and loses the separately held lifetime offset at 080FBEEE. Scope
also identifies the conflict between allocation candidates 70 and 72.

AxeArmor_UpdateCountdown (08204E5C) updates the countdown field
directly. AxeArmor_RecordDefeat (08204748) writes the five existing
game-state reaction counters while preserving saturation. Direct
sequence flips still change the build. Combining the position stores
for the already-dead state shortens that initializer by twelve bytes.

The third pass reduces the six-object reports from 646 to 644 rows
and the steering subtotal from 705 to 702 uses. The six object reports
contain zero errors and the comparison found no new diagnostics.

All nine changed source units match their recorded ROM extents.
