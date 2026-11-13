#ifndef GUARD_ACTOR_STATE_COMMANDS_H
#define GUARD_ACTOR_STATE_COMMANDS_H
#include "global.h"
#include "actor_state.h"
#include "engine_globals.h"
#include "ram.h"
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
