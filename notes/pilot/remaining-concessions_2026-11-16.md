# Functions still carrying a concession after pilot batch 17 (2026-11-16)

From `python3 tools/progress.py --list-pinned` on main after batch 17: 36 functions, in ROM order.

| Address | Function | Kinds | Bytes |
|---|---|---|---|
| 08012B34 | sub_08012B34 | pin | 468 |
| 08014330 | ParticleGroup_UpdateFrozen | operand | 360 |
| 08014634 | sub_08014634 | pin,operand | 664 |
| 08014CA4 | sub_08014CA4 | pin,operand | 704 |
| 080154EC | AnimatedParticles_Create | operand | 1176 |
| 08016118 | FadingParticleGroup_Update | pin,barrier | 540 |
| 0803198C | sub_0803198C | pin | 948 |
| 08038448 | WirelessGameMenu_InitEquipmentChoices | pin,operand | 408 |
| 0803B40C | WirelessResultPanel_Init | pin,operand | 684 |
| 0804373C | ContactActor_UpdateRetreat | pin,operand | 788 |
| 0804CB4C | TetherActor_UpdateBounceReturn | pin | 356 |
| 08051EA8 | ProximityEmitter_InitializeFromScript | pin | 576 |
| 0805AC3C | ScenePanel_TickRig | pin,operand | 220 |
| 0805B0F0 | ScenePanel_InitRig | operand | 412 |
| 0805E1D0 | SceneInput_TickChoice | operand | 160 |
| 08071F50 | SceneActorPalette_ApplyTransition | pin,operand | 468 |
| 08072D34 | SceneActorContacts_ReleaseInvalidOwners | pin,operand | 212 |
| 080A4018 | Spider_UpdateWanderGoal | pin | 584 |
| 080AA8C4 | TentacleActor_AlignSegmentsToTerrain | operand | 460 |
| 080AB968 | TentacleActor_UpdateRetraction | operand | 324 |
| 080AEE08 | Vorn_UpdateDefeat | single-use-macro | 984 |
| 080BD1F0 | Skeleton_UpdateDefeat | pin | 7924 |
| 080D1784 | Mummy_ResumeSavedRouteAfterAnimation | pin,operand | 788 |
| 08101B1C | NamedObject_RegisterFollowCells | pin | 528 |
| 08108C94 | sub_08108C94 | pin | 456 |
| 0815F5B4 | TargetRegistry_FindById | pin | 68 |
| 0817AEC8 | sub_0817AEC8 | pin | 288 |
| 0819BB18 | sub_0819BB18 | operand | 416 |
| 0819BCB8 | Hresvelgr_EmitFacingMotion | pin,operand | 368 |
| 081A0BC8 | Hresvelgr_UpdateBoundaryBarrage | operand | 732 |
| 081A14FC | sub_081A14FC | pin,operand | 1764 |
| 081D3620 | sub_081D3620 | pin | 132 |
| 0820EE34 | OrangeSpine_ApplyHitReaction | pin,operand | 424 |
| 08217430 | sub_08217430 | pin,operand | 1328 |
| 0821B084 | GameState_SelectRamBase | pin | 88 |
| 08234540 | SpiralBurst_InitParticles | pin | 428 |
