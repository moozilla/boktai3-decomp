# Golem, Dog, Axe and Axe Armor layouts

Tags: #monsters #types #phase3 #matching

## Golem and Dog

Golem_ResetCombatState now clears the original fields through the 0x6B0
Golem type. The slide and charge buildup share the typed speed and route
fields. Golem_UpdateScriptedAction uses the gate duration and counter at
+0x416 and +0x418. The shared state transitions preserve control entry.
Steering debt fell from 136 to 130 uses.

Dog_Create uses the checked 0x6E0 allocation type. The reaction path only
uses Dog fields, so the duplicate union is gone. The full recorded Dog unit
matches. The direct form in SceneActor_ConfigureSecondaryProfileEntry
(080DE92C) moves the zero before the value 4 and shares it with another
byte assignment. The ROM instead forms the secondary record pointer after
both values. The pointer and conditional store trials leave that decision
open. Dog's steering-debt count stayed at 134.

## Axe and Axe Armor

Axe_Init sets the chase group's route probe and destination to the storage
at +0xED8 and +0xEFC. Axe_Create now uses the checked 0xF24 type.
Axe_EmitMotionTrail reads its script resource through the 0x60-byte slot.
The indexed rewrite in Axe_OnBodyContact changes the address induction and
reload set. Axe_FireAttack still executes a separate flag test before two
identical groups of calls. Collapsing that branch changes the ROM.
Axe's steering debt fell from 99 to 98 uses.

AxeArmor_Create (08205900) allocates a 0x198 task. The actor type combines
the drawable, graphics, collider, scene body, sequence and fixed eight-entry
attack schedule already read by the initializer and update routines.
AxeArmor_Update (08204F40) matches when the damage number uses the owning
actor's collider position and offset. Using the selected collider pointer
for those two arguments changes four instructions. The initializer
(0820539C) uses the shared type and keeps the callback stores through their
original pointer form to preserve reload order. The shared display step
changes code generation. Axe Armor's steering debt fell from 13 to 12 uses.

## Checking the worker changes

The stock unit checks preserve the recorded ROM ranges. All affected header
dependents match, including ten consumers of the Axe Armor type.

The keep-or-revert tools use the checker's verdict-only mode for trial
compiles. It compares the same linked bytes and honors the reviewed extent
when supplied. On three repetitions per mode, the median check of the Dog
unit fell from 1.326 to 0.993 seconds. The comparison and keep-or-revert
tests passed.
