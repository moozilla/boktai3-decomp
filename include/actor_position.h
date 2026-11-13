#ifndef GUARD_ACTOR_POSITION_H
#define GUARD_ACTOR_POSITION_H
#include "actor_state.h"
#include "engine.h"
union ActorPositionVector {
    struct ContactPosition position;
    s16 components[4];
};
s32 Mod(s32, s32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
s32 ActorPosition_TestPath(struct ActorState *, struct ContactPosition *);
#endif
