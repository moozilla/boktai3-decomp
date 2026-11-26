#ifndef GUARD_SABATA_POSITION_H
#define GUARD_SABATA_POSITION_H
#include "bosses/sabata/sabata_actor_state.h"
#include "engine/engine.h"
u8 Terrain_IsSegmentBelowHeightLimit(void *, void *, u32, u32);
s32 ActorMovement_CanReachTarget(struct SabataActorState *);
s32 Mod(s32, s32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
s32 SabataPosition_TestPath(struct SabataActorState *, struct ContactPosition *);
#endif
