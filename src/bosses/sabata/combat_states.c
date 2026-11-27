#include "bosses/sabata/sabata_combat_states.h"

static inline s32 GetPulseTrigger(struct SabataActorStateOwner *owner, u8 **trigger)
{
    *trigger = &owner->actor.triggered;
    return **trigger;
}

static inline s32 RandomValue(void)
{
    u16 *values = gRandomTable;
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)values);
}

void Sabata_UpdateTimedAttackChoice(struct SabataActorState *actor)
{
    struct SabataActorState *stateData = actor;
    if (SabataState_TakeEntryTrigger(actor)) {
        stateData->animationState = 3;
        stateData->movementTarget = stateData->nearbyPosition;
        {
            u32 phase = 5;
            actor->state = phase;
        }
        stateData->animationPhase = 0;
    }
    if (actor->timer == 30) {
        stateData->animationState = 3;
        {
            u32 phase = 4;
            actor->state = phase;
        }
    } else if (ActorMovement_CanReachTarget(stateData))
        Sabata_BeginCombatPhase(actor, 1);
    else if (Sabata_GetAttackActivity(actor))
        Sabata_BeginCombatPhase(actor, 2);
    else if (actor->timer > 60 && Sabata_FindNearbyAttackPosition(stateData) == 1)
        Sabata_BeginCombatPhase(actor, 3);
    else if (actor->timer > 600 || Sabata_TakeReachedTarget(stateData)) {
        Sabata_RequestRestart(actor);
    } else if (actor->pending) {
        stateData->animationState = 3;
        {
            u32 phase = 4;
            actor->state = phase;
        }
    }
    actor->timer++;
}

void Sabata_WaitBeforeRestart(struct SabataActorState *actor)
{
    struct SabataActorState *stateData = actor;
    if (SabataState_TakeEntryTrigger(actor)) {
        u32 phase = 5;
        actor->state = phase;
    }
    stateData->decisionTimeout = 600;
    if (actor->timer > 30) {
        Sabata_RequestRestart(actor);
    }
    actor->timer++;
}

void Sabata_UpdateTurningAttackChoice(struct SabataActorStateOwner *input)
{
    struct SabataActorStateOwner *owner = input;
    struct SabataActorStateOwner *actor;
    u8 *trigger;
    s32 result;
    actor = input;
    if (GetPulseTrigger(owner, &trigger)) {
        actor = (struct SabataActorStateOwner *)(trigger - ((u8 *)&owner->actor.triggered - (u8 *)owner));
        actor->actor.triggered = 0;
        owner->actor.pending = 0;
    }
    if (ActorMovement_CanReachTarget(&actor->actor))
        Sabata_BeginCombatPhase(&owner->actor, 1);
    else if (Sabata_GetAttackActivity(&owner->actor))
        Sabata_BeginCombatPhase(&owner->actor, 2);
    else if (owner->actor.timer > 60 && Sabata_FindNearbyAttackPosition(&actor->actor) == 1)
        Sabata_BeginCombatPhase(&owner->actor, 3);
    else if (owner->actor.timer > 210) {
        Sabata_RequestRestart(&owner->actor);
    }
    if (actor->actor.requestMode == 2)
        result = ActorEffect_StepTurn(&owner->actor, owner->actor.timer, 30, 30, 1);
    else
        result = ActorEffect_StepTurn(&owner->actor, owner->actor.timer, 30, 30, 0);
    if (result) {
        u32 phase = 6;
        owner->actor.state = phase;
    }
    owner->actor.timer++;
}

void Sabata_FollowHazardPath(struct SabataActorState *actor)
{
    struct SabataActorState *path = actor;
    if (SabataState_TakeEntryTrigger(actor)) {
        path->animationState = 3;
        path->animationIndex = (RandomValue() >> 8) & 3;
        path->animationDetail = Mod(RandomValue(), 5);
        Path_GetPointPositionById(&path->movementTarget, path->animationIndex,
                                  path->animationDetail);
        {
            u32 state = 4;
            actor->state = state;
        }
        {
            path->attackFlags |= 2;
        }
    }
    if (Sabata_GetAttackActivity(actor))
        Sabata_BeginCombatPhase(actor, 2);
    if (actor->pending == 1) {
        if (Sabata_TakeReachedTarget(path)) {
            path->animationDetail++;
            if (++actor->timer > 3) {
                Sabata_RequestRestart(actor);
            } else
                AnimatedHazard_CreateProximity(&actor->body.position, path->hazardParameter, 800,
                                               60);
            Path_GetPointPositionById(&path->movementTarget, path->animationIndex,
                                      path->animationDetail);
            path->animationState = 3;
            {
                u32 state = 4;
                actor->state = state;
            }
        } else {
            path->animationState = 3;
            {
                u32 state = 4;
                actor->state = state;
            }
        }
    }
}

void Sabata_WaitForAttackActivity(struct SabataActorState *actor)
{
    struct SabataActorState *stateData = actor;
    s32 result;
    if (SabataState_TakeEntryTrigger(actor)) {
        u32 phase = 8;
        actor->state = phase;
    }
    result = Sabata_GetAttackActivity(actor);
    if (result) {
        Sabata_BeginCombatPhase(actor, 2);
        stateData->attackFlags |= 1;
    } else if (actor->timer > 600) {
        Sabata_RequestRestart(actor);
    }
    actor->timer++;
}

void SceneActorState_UpdateAnimationRequest(struct SabataActorState *actor)
{
    if (SabataState_TakeEntryTrigger(actor)) {
        u32 state = 7;
        actor->state = state;
    }
    if (actor->pending == 1) {
        actor->state = 1;
        Sabata_RequestRestart(actor);
    }
    actor->timer++;
}

void Sabata_WaitForPendingAttackEvent(struct SabataActorState *actor)
{
    struct SabataActorState *stateData = actor;
    if (SabataState_TakeEntryTrigger(actor)) {
        u32 phase = 2;
        actor->state = phase;
    } else if (actor->pending == 1) {
        if (stateData->transitionBlocked)
            Sabata_BeginCombatPhase(actor, 1);
        else {
            Sabata_RequestRestart(actor);
        }
    }
    actor->timer++;
}

void Sabata_UpdateAttackThreshold(struct SabataActorState *actor)
{
    struct SabataActorState *stateData = actor;
    if (SabataState_TakeEntryTrigger(actor)) {
        stateData->animationState = 3;
        ActorMovement_FindQuarterTurnPoint(stateData, &stateData->movementTarget);
        {
            u32 phase = 4;
            actor->state = phase;
        }
    } else if (actor->timer < 60 && actor->threshold > 0x10000) {
        if (actor->pending == 1) {
            stateData->animationState = 3;
            ActorMovement_FindQuarterTurnPoint(stateData, &stateData->movementTarget);
            {
                u32 phase = 4;
                actor->state = phase;
            }
        }
    } else if (actor->controlPhase == 3) {
        if (actor->pending == 1) {
            Sabata_RequestRestart(actor);
        }
    } else if (actor->pending == 1) {
        if (stateData->requestMode == 0)
            stateData->animationState = 0;
        else
            stateData->animationState = 1;
        {
            u32 phase = 3;
            actor->state = phase;
        }
    }
    if (Sabata_GetAttackActivity(actor) == 2)
        Sabata_BeginCombatPhase(actor, 2);
    actor->timer++;
}

void Sabata_ClearCombatEntryTrigger(struct SabataActorState *actor)
{
    SabataState_TakeEntryTrigger(actor);
}

void Sabata_WaitForVariantTrigger(struct SabataActorState *actor)
{
    u32 mask;
    u32 flags;
    if (SabataState_TakeEntryTrigger(actor)) {
        u32 state = 0xc;
        actor->state = state;
    }
    mask = 1;
    flags = actor->flags & mask;
    if (flags == 0) {
        u32 variant = 0xe;
        actor->active = flags;
        actor->phase = flags;
        actor->variant = variant;
        actor->timer = flags;
        actor->triggered = mask;
        actor->state = mask;
    } else
        actor->timer++;
}

void SabataState_UpdateTrigger(struct SabataActorState *actor)
{
    struct SabataActorState *actorData = actor;
    if (SabataState_TakeEntryTrigger(actor)) {
        u32 state = 9;
        actor->state = state;
        ActorExtension_ResetState(actorData);
        actorData->hitParameter = 0x70;
    }
    if (actor->pending) {
        Sabata_RequestRestart(actor);
    }
    actor->timer++;
}

void SabataState_UpdateConditionalTrigger(struct SabataActorState *actor)
{
    if (SabataState_TakeEntryTrigger(actor)) {
        u32 state = 10;
        actor->state = state;
    }
    if (Sabata_GetAttackActivity(actor)) {
        Sabata_BeginCombatPhase(actor, 2);
    } else if (actor->pending) {
        Sabata_RequestRestart(actor);
    }
    actor->timer++;
}
