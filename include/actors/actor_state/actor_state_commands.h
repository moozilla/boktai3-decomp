#ifndef GUARD_ACTOR_STATE_COMMANDS_H
#define GUARD_ACTOR_STATE_COMMANDS_H
#include "engine/shared/global.h"
#include "engine/shared/actor_state.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"

static inline u8 ActorState_TakeEntryTrigger(struct ActorState *actor)
{
    if (actor->triggered) {
        actor->triggered = 0;
        actor->pending = 0;
        return 1;
    }
    return 0;
}

static inline void SceneVolume_RequestRestart(struct ActorState *actor)
{
    actor->requestedVariant = actor->variant;
    actor->request = 1;
    actor->requestPhase = 0;
}

s32 sub_08249350(s32, s32);
s32 ActorMovement_CanReachTarget(struct ActorState *);
s32 SceneVolume_GetActivityState(struct ActorState *);
void SceneVolume_BeginPhase(struct ActorState *, s32);
s32 SceneVolume_FindNearbyPosition(struct ActorState *);
s32 ActorEffect_StepTurn(struct ActorState *, s32, s32, s32, s32);
s32 Path_PickRandomPoint(void *, u32);
void ActorImpact_FindNearbyPoint(struct ActorState *, struct ContactPosition *, s32);
s32 SceneVolume_ConsumePendingEvent(struct ActorState *);
void ActorVariation_SelectRandomPair(struct ActorState *, struct ContactPosition *);
s32 ActorPosition_TestPath(struct ActorState *, struct ContactPosition *);
#endif
