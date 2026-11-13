#include "actor_decision.h"

static inline u8 ActorDecision_ConsumeTrigger(struct ActorState *actor)
{
    if (actor->triggered) {
        actor->triggered = 0;
        actor->pending = 0;
        return 1;
    }
    return 0;
}

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

void ActorDecision_UpdateTimedPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    if (ActorDecision_ConsumeTrigger(actor)) {
        stateData->animationState = 3;
        stateData->animation = stateData->targetAnimation;
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
        SceneVolume_BeginPhase(actor, 1);
    else if (SceneVolume_GetActivityState(actor))
        SceneVolume_BeginPhase(actor, 2);
    else if (actor->timer > 60 && SceneVolume_FindNearbyPosition(stateData) == 1)
        SceneVolume_BeginPhase(actor, 3);
    else if (actor->timer > 600 || SceneVolume_ConsumePendingEvent(stateData)) {
        actor->requestedVariant = actor->variant;
        actor->request = 1;
        actor->requestPhase = 0;
    } else if (actor->pending) {
        stateData->animationState = 3;
        {
            u32 phase = 4;
            actor->state = phase;
        }
    }
    actor->timer++;
}

void ActorDecision_UpdateDelayPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    if (ActorDecision_ConsumeTrigger(actor)) {
        u32 phase = 5;
        actor->state = phase;
    }
    stateData->decisionTimeout = 600;
    if (actor->timer > 30) {
        actor->requestedVariant = actor->variant;
        actor->request = 1;
        actor->requestPhase = 0;
    }
    actor->timer++;
}

void ActorDecision_UpdatePulsePhase(struct ActorStateOwner *input)
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
        SceneVolume_BeginPhase(&owner->actor, 1);
    else if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (owner->actor.timer > 60 && SceneVolume_FindNearbyPosition(&actor->actor) == 1)
        SceneVolume_BeginPhase(&owner->actor, 3);
    else if (owner->actor.timer > 210) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
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

void ActorDecision_UpdateHazardPath(struct ActorState *actor)
{
    struct ActorState *path = actor;
    if (ActorDecision_ConsumeTrigger(actor)) {
        path->animationState = 3;
        path->animationIndex = (RandomValue() >> 8) & 3;
        path->animationDetail = Mod(RandomValue(), 5);
        Path_GetPointPositionById(&path->animation, path->animationIndex, path->animationDetail);
        {
            u32 state = 4;
            actor->state = state;
        }
        {
            u32 mask = 2;
            path->attackFlags |= mask;
        }
    }
    if (SceneVolume_GetActivityState(actor))
        SceneVolume_BeginPhase(actor, 2);
    if (actor->pending == 1) {
        if (SceneVolume_ConsumePendingEvent(path)) {
            path->animationDetail++;
            if (++actor->timer > 3) {
                actor->requestedVariant = actor->variant;
                actor->request = 1;
                actor->requestPhase = 0;
            } else
                AnimatedHazard_CreateProximity(&actor->body.position, path->hazardParameter, 800, 60);
            Path_GetPointPositionById(&path->animation, path->animationIndex, path->animationDetail);
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

void ActorDecision_UpdateFlagPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    s32 result;
    if (ActorDecision_ConsumeTrigger(actor)) {
        u32 phase = 8;
        actor->state = phase;
    }
    result = SceneVolume_GetActivityState(actor);
    if (result) {
        SceneVolume_BeginPhase(actor, 2);
        stateData->attackFlags |= 1;
    } else if (actor->timer > 600) {
        actor->requestedVariant = actor->variant;
        actor->request = 1;
        actor->requestPhase = 0;
    }
    actor->timer++;
}

void SceneActorState_UpdateAnimationRequest(struct ActorState *actor)
{
    if (ActorDecision_ConsumeTrigger(actor)) {
        u32 state = 7;
        actor->state = state;
    }
    if (actor->pending == 1) {
        actor->state = 1;
        actor->requestedVariant = actor->variant;
        actor->request = 1;
        actor->requestPhase = 0;
    }
    actor->timer++;
}

void ActorDecision_UpdatePendingPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    if (ActorDecision_ConsumeTrigger(actor)) {
        u32 phase = 2;
        actor->state = phase;
    } else if (actor->pending == 1) {
        if (stateData->transitionBlocked)
            SceneVolume_BeginPhase(actor, 1);
        else {
            actor->requestedVariant = actor->variant;
            actor->request = 1;
            actor->requestPhase = 0;
        }
    }
    actor->timer++;
}

void ActorDecision_UpdateThresholdPhase(struct ActorState *actor)
{
    struct ActorState *stateData = actor;
    if (ActorDecision_ConsumeTrigger(actor)) {
        stateData->animationState = 3;
        ActorMovement_FindQuarterTurnPoint(stateData, &stateData->animation);
        {
            u32 phase = 4;
            actor->state = phase;
        }
    } else if (actor->timer < 60 && actor->threshold > 0x10000) {
        if (actor->pending == 1) {
            stateData->animationState = 3;
            ActorMovement_FindQuarterTurnPoint(stateData, &stateData->animation);
            {
                u32 phase = 4;
                actor->state = phase;
            }
        }
    } else if (actor->controlPhase == 3) {
        if (actor->pending == 1) {
            actor->requestedVariant = actor->variant;
            actor->request = 1;
            actor->requestPhase = 0;
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
    if (SceneVolume_GetActivityState(actor) == 2)
        SceneVolume_BeginPhase(actor, 2);
    actor->timer++;
}

void ActorDecision_ClearTrigger(struct ActorState *actor)
{
    if (actor->triggered != 0) {
        actor->triggered = 0;
        actor->pending = 0;
    }
}

void ActorDecision_UpdateFlaggedWait(struct ActorState *actor)
{
    u32 mask;
    u32 flags;
    if (ActorDecision_ConsumeTrigger(actor)) {
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

void ActorState_UpdateTrigger(struct ActorState *actor)
{
    struct ActorState *actorData = actor;
    if (ActorDecision_ConsumeTrigger(actor)) {
        u32 state = 9;
        actor->state = state;
        ActorExtension_ResetState(actorData);
        actorData->hitParameter = 0x70;
    }
    if (actor->pending) {
        actor->requestedVariant = actor->variant;
        actor->request = 1;
        actor->requestPhase = 0;
    }
    actor->timer++;
}

void ActorState_UpdateConditionalTrigger(struct ActorState *actor)
{
    if (ActorDecision_ConsumeTrigger(actor)) {
        u32 state = 10;
        actor->state = state;
    }
    if (SceneVolume_GetActivityState(actor)) {
        SceneVolume_BeginPhase(actor, 2);
    } else if (actor->pending) {
        actor->requestedVariant = actor->variant;
        actor->request = 1;
        actor->requestPhase = 0;
    }
    actor->timer++;
}
