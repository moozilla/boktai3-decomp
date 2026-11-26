#include "bosses/sabata/actor_decision.h"

static inline s32 GetPulseTrigger(struct ActorStateOwner *owner, u8 **trigger)
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

void SabataDecision_UpdateTimedPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    if (ActorState_TakeEntryTrigger(actor)) {
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
        SabataVolume_BeginPhase(actor, 1);
    else if (SabataVolume_GetActivityState(actor))
        SabataVolume_BeginPhase(actor, 2);
    else if (actor->timer > 60 && SabataVolume_FindNearbyPosition(stateData) == 1)
        SabataVolume_BeginPhase(actor, 3);
    else if (actor->timer > 600 || SabataVolume_ConsumePendingEvent(stateData)) {
        SceneVolume_RequestRestart(actor);
    } else if (actor->pending) {
        stateData->animationState = 3;
        {
            u32 phase = 4;
            actor->state = phase;
        }
    }
    actor->timer++;
}

void SabataDecision_UpdateDelayPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    if (ActorState_TakeEntryTrigger(actor)) {
        u32 phase = 5;
        actor->state = phase;
    }
    stateData->decisionTimeout = 600;
    if (actor->timer > 30) {
        SceneVolume_RequestRestart(actor);
    }
    actor->timer++;
}

void SabataDecision_UpdatePulsePhase(struct ActorStateOwner *input)
{
    struct ActorStateOwner *owner = input;
    struct ActorStateOwner *actor;
    u8 *trigger;
    s32 result;
    actor = input;
    if (GetPulseTrigger(owner, &trigger)) {
        actor = (struct ActorStateOwner *)(trigger - ((u8 *)&owner->actor.triggered - (u8 *)owner));
        actor->actor.triggered = 0;
        owner->actor.pending = 0;
    }
    if (ActorMovement_CanReachTarget(&actor->actor))
        SabataVolume_BeginPhase(&owner->actor, 1);
    else if (SabataVolume_GetActivityState(&owner->actor))
        SabataVolume_BeginPhase(&owner->actor, 2);
    else if (owner->actor.timer > 60 && SabataVolume_FindNearbyPosition(&actor->actor) == 1)
        SabataVolume_BeginPhase(&owner->actor, 3);
    else if (owner->actor.timer > 210) {
        SceneVolume_RequestRestart(&owner->actor);
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

void SabataDecision_UpdateHazardPath(struct ActorState *actor)
{
    struct ActorState *path = actor;
    if (ActorState_TakeEntryTrigger(actor)) {
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
    if (SabataVolume_GetActivityState(actor))
        SabataVolume_BeginPhase(actor, 2);
    if (actor->pending == 1) {
        if (SabataVolume_ConsumePendingEvent(path)) {
            path->animationDetail++;
            if (++actor->timer > 3) {
                SceneVolume_RequestRestart(actor);
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

void SabataDecision_UpdateFlagPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    s32 result;
    if (ActorState_TakeEntryTrigger(actor)) {
        u32 phase = 8;
        actor->state = phase;
    }
    result = SabataVolume_GetActivityState(actor);
    if (result) {
        SabataVolume_BeginPhase(actor, 2);
        stateData->attackFlags |= 1;
    } else if (actor->timer > 600) {
        SceneVolume_RequestRestart(actor);
    }
    actor->timer++;
}

void SceneActorState_UpdateAnimationRequest(struct ActorState *actor)
{
    if (ActorState_TakeEntryTrigger(actor)) {
        u32 state = 7;
        actor->state = state;
    }
    if (actor->pending == 1) {
        actor->state = 1;
        SceneVolume_RequestRestart(actor);
    }
    actor->timer++;
}

void SabataDecision_UpdatePendingPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    if (ActorState_TakeEntryTrigger(actor)) {
        u32 phase = 2;
        actor->state = phase;
    } else if (actor->pending == 1) {
        if (stateData->transitionBlocked)
            SabataVolume_BeginPhase(actor, 1);
        else {
            SceneVolume_RequestRestart(actor);
        }
    }
    actor->timer++;
}

void SabataDecision_UpdateThresholdPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    if (ActorState_TakeEntryTrigger(actor)) {
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
            SceneVolume_RequestRestart(actor);
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
    if (SabataVolume_GetActivityState(actor) == 2)
        SabataVolume_BeginPhase(actor, 2);
    actor->timer++;
}

void SabataDecision_ClearTrigger(struct ActorState *actor)
{
    ActorState_TakeEntryTrigger(actor);
}

void SabataDecision_UpdateFlaggedWait(struct ActorState *actor)
{
    u32 mask;
    u32 flags;
    if (ActorState_TakeEntryTrigger(actor)) {
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

void SabataState_UpdateTrigger(struct ActorState *actor)
{
    struct ActorState *actorData = actor;
    if (ActorState_TakeEntryTrigger(actor)) {
        u32 state = 9;
        actor->state = state;
        ActorExtension_ResetState(actorData);
        actorData->hitParameter = 0x70;
    }
    if (actor->pending) {
        SceneVolume_RequestRestart(actor);
    }
    actor->timer++;
}

void SabataState_UpdateConditionalTrigger(struct ActorState *actor)
{
    if (ActorState_TakeEntryTrigger(actor)) {
        u32 state = 10;
        actor->state = state;
    }
    if (SabataVolume_GetActivityState(actor)) {
        SabataVolume_BeginPhase(actor, 2);
    } else if (actor->pending) {
        SceneVolume_RequestRestart(actor);
    }
    actor->timer++;
}
