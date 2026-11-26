#ifndef GUARD_SABATA_STATE_COMMANDS_H
#define GUARD_SABATA_STATE_COMMANDS_H
#include "global.h"
#include "bosses/sabata/sabata_actor_state.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

static inline u8 SabataState_TakeEntryTrigger(struct SabataActorState *actor)
{
    if (actor->triggered) {
        actor->triggered = 0;
        actor->pending = 0;
        return 1;
    }
    return 0;
}

static inline void Sabata_RequestRestart(struct SabataActorState *actor)
{
    actor->requestedVariant = actor->variant;
    actor->request = 1;
    actor->requestPhase = 0;
}

s32 sub_08249350(s32, s32);
s32 ActorMovement_CanReachTarget(struct SabataActorState *);
s32 Sabata_GetAttackActivity(struct SabataActorState *);
void Sabata_BeginCombatPhase(struct SabataActorState *, s32);
s32 Sabata_FindNearbyAttackPosition(struct SabataActorState *);
s32 ActorEffect_StepTurn(struct SabataActorState *, s32, s32, s32, s32);
s32 Path_PickRandomPoint(void *, u32);
void SabataImpact_FindNearbyPoint(struct SabataActorState *, struct ContactPosition *, s32);
s32 Sabata_TakeReachedTarget(struct SabataActorState *);
void ActorVariation_SelectRandomPair(struct SabataActorState *, struct ContactPosition *);
s32 SabataPosition_TestPath(struct SabataActorState *, struct ContactPosition *);
#endif
