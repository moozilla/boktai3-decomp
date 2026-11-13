#ifndef GUARD_ACTOR_POSITION_H
#define GUARD_ACTOR_POSITION_H
#include "actor_state.h"
#include "engine.h"
union ActorPositionVector {
    struct ContactPosition position;
    s16 components[4];
};
u8 sub_0821D4F0(void *, void *, u32, u32);
s32 ActorMovement_CanReachTarget(struct ActorState *);
s32 Mod(s32, s32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
s32 ActorPosition_TestPath(struct ActorState *, struct ContactPosition *);
#endif
