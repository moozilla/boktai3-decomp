# Centipede and Armor actor cleanup

Tags: #monsters #types #phase3 #matching

## Centipede

The 0x934-byte body used by Centipede_Create (080ACBA8) now owns the
route history and its marked segment's scene body. Centipede_ResetRouteHistory
(080AB4E0) and Centipede_HasVisitedRoute (080AB51C) use six slots.
Centipede_RecordVisitedRoute (080AB54C) advances the write index modulo four.
The reset preserves the terrain cell read on each iteration.

Centipede_ActivateMarkedSegment (080A985C) passes the object at context +0x888
to SceneBody_Init, then selects the segment as its sprite. The target update
at 080A97AC uses the same object's position and sprite fields. Other units now
use the shared segment stride and typed positions for headings, interpolation,
motion, collider removal and animation updates.

The waypoint variants (080A7700, 080A79E8, 080A7C84) and spread attacks use
the existing fixed-point projection. SceneActor_TryRequestFacingReaction
(080AB604) uses the typed tracking query documented in the flag-order survey.
Steering debt fell from 194 to 157 uses across the Centipede scope.

The indexed loop at 080A95F8 materializes the segment base and counter in a
different order. Its reload set also loses the register the ROM uses for the
count offset. In the marked-segment activation candidate, using an alias for
the embedded scene body's sprite store changed the reload rotation. Referencing
the embedded field through its owner matched the entire unit.

## Armor knights

Armor_InitFromScript (0818C0BC) passes offset 0xB58 to Armor_InitParts
(0818E5E4). The existing 0x5C8 ArmorLaunchGroup matches this embedded field
in the 0x1134-byte SceneCombatActor. Its three records have a 0x1E4 stride.
The part motion entry points (0818E170, 0818E1BC, 0818E200, 0818E244) write
the phase at +0x12E and update callback at +0x1D0 through the shared type.
Armor_IsSequenceIncomplete (0818EA1C) reads the initialized group counts.

The charge routines and collider initializer now use the shared Armor actor.
Direct paired reaction clears in 081885BC and its four related handlers move
mask construction relative to collider address formation. In the part spin
entry, replacing the parked bit with the direct flag expression moves its
construction after the load and changes the reload set. The fields stay
typed and the matching mask forms remain visible for review. Steering debt
fell from 194 to 186 uses across the Armor scope.

Every changed unit matched under the stock compiler. Header-dependent checks
covered the Centipede body, the Armor actor and the Armor part group.
