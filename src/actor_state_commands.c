#include "actor_state_commands.h"

static inline u8 ActorState_ConsumeTrigger(struct ActorState *actor)
{
    if (actor->triggered) {
        actor->triggered = 0;
        actor->pending = 0;
        return 1;
    }
    return 0;
}

void SceneVolume_BeginPhase(struct ActorState *volume, s32 phase)
{
    volume->requestedVariant = volume->variant;
    volume->requestPhase = phase;
}

static inline void update_facing(struct ActorState *actor, u8 *facingDirection)
{
    s32 direction = (*facingDirection + 96) >> 6;
    u8 mask = 3;
    do {
        mask &= direction;
    } while (0);
    actor->facing4 = mask;
    direction = (*facingDirection + 112) >> 5;
    mask = 7;
    mask &= direction;
    actor->facing8 = mask;
}

s32 ActorEffect_StepTurn(struct ActorState *actor, s32 frame, s32 start, s32 period,
                         s32 mode)
{
    s32 elapsed = frame - start;
    s32 step = elapsed / period;
    s32 remainder;
    u8 *direction;
    if (elapsed < 0)
        return 0;
    remainder = sub_08249350(elapsed, period);
    switch (mode) {
        case 0:
            if (remainder != 0)
                return 0;
            if (step == 4)
                return 1;
            if (step > 4)
                return 0;
            if ((step == 0) || (step == 3)) {
                direction = &actor->body.direction;
                *direction += 32;
            } else
                goto subtract;
            break;

        case 1: {
            s32 quarter = period >> 2;
            if ((remainder != 0) && (remainder != quarter))
                return 0;
        }
            if ((step == 4) && (remainder == 0))
                return 1;
            if (step > 4)
                return 0;
            if ((step == 0) || (step == 3)) {
                direction = &actor->body.direction;
                *direction += 32;
            } else
                goto subtract;
            break;

        default:
            goto animate;
    }

    goto facing;
subtract:
    direction = &actor->body.direction;
    *direction -= 32;

facing:
    update_facing(actor, direction);

animate: {
    u32 animation = 1;
    actor->state = animation;
}

    return 0;
}

s32 ActorState_BeginDepletedState(struct ActorState *actor)
{
    s32 result;
    actor->checking = 1;
    if (actor->vitality <= 0) {
        u32 mask = 2;
        actor->flags |= mask;
        {
            u32 state = 13;
            actor->state = state;
        }
    }
    actor->checking = 0;
    {
        u32 mask = 2;
        if (!(actor->flags & mask))
            result = 0;
        else {
            u32 variant = 3;
            actor->active = 1;
            actor->phase = 0;
            actor->variant = variant;
            actor->timer = 0;
            actor->triggered = 1;
            result = 1;
        }
    }
    return result;
}

void ActorState_UpdateDepletedState(struct ActorStateOwner *owner)
{
    struct ActorState *actor = &owner->actor;
    if (ActorState_ConsumeTrigger(&owner->actor)) {
        u32 state = 1;
        owner->actor.state = state;
    }
    if (ActorMovement_CanReachTarget(actor))
        SceneVolume_BeginPhase(&owner->actor, 1);
    else if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (owner->actor.timer > 60) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
    }
    owner->actor.timer++;
}

static inline s32 RandomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gRandomTable);
}

void ActorState_UpdateRandomAnimation(struct ActorStateOwner *owner)
{
    struct ActorState *actor = &owner->actor;
    s32 duration;
    s32 result;
    if (ActorState_ConsumeTrigger(actor)) {
        actor->animationIndex = (RandomValue() >> 8) & 3;
        if (Path_PickRandomPoint(&actor->movementTarget, actor->animationIndex) >= 0)
            actor->animationState = 3;
        {
            u32 state = 4;
            owner->actor.state = state;
        }
    }
    duration = actor->mode == 1 ? 20 : 60;
    if (ActorMovement_CanReachTarget(actor))
        SceneVolume_BeginPhase(&owner->actor, 1);
    else if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (SceneVolume_FindNearbyPosition(actor) == 1)
        SceneVolume_BeginPhase(&owner->actor, 3);
    if (actor->requestMode == 2)
        result = ActorEffect_StepTurn(&owner->actor, owner->actor.timer, duration, duration, 1);
    else
        result = ActorEffect_StepTurn(&owner->actor, owner->actor.timer, duration, duration, 0);
    if (result) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
    }
    owner->actor.timer++;
}

void ActorState_UpdateSelectedAnimation(struct ActorStateOwner *owner)
{
    struct ActorState *actor = &owner->actor;
    s32 result;
    if (ActorState_ConsumeTrigger(&owner->actor)) {
        if (!ActorPosition_TestPath(actor, &actor->savedPosition)) {
            owner->actor.requestedVariant = owner->actor.variant;
            owner->actor.request = 1;
            owner->actor.requestPhase = 0;
            owner->actor.state = 1;
            return;
        }
        actor->animationState = 3;
        actor->movementTarget = actor->savedPosition;
        {
            u32 state = 4;
            owner->actor.state = state;
        }
    }
    if (ActorMovement_CanReachTarget(actor))
        SceneVolume_BeginPhase(&owner->actor, 1);
    else if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (owner->actor.timer > 60 && SceneVolume_FindNearbyPosition(actor) == 1)
        SceneVolume_BeginPhase(&owner->actor, 3);
    if (actor->requestMode == 2)
        result = ActorEffect_StepTurn(&owner->actor, owner->actor.timer, 30, 10, 1);
    else
        result = ActorEffect_StepTurn(&owner->actor, owner->actor.timer, 30, 10, 0);
    if (result) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
    }
    owner->actor.timer++;
}

void ActorState_BeginReverseAnimation(struct ActorStateOwner *owner)
{
    struct ActorState *actor = &owner->actor;
    if (ActorState_ConsumeTrigger(&owner->actor)) {
        actor->animationState = 3;
        ActorImpact_FindNearbyPoint(actor, &actor->movementTarget, (actor->direction + 128) & 255);
        {
            u32 state = 4;
            owner->actor.state = state;
        }
    }
    if (SceneVolume_ConsumePendingEvent(actor)) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
        owner->actor.state = 1;
    } else if (owner->actor.pending) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
        owner->actor.state = 1;
    }
}

void ActorState_UpdateRestartableAnimation(struct ActorStateOwner *owner)
{
    struct ActorState *actor = &owner->actor;
    u32 nextState;
    if (ActorState_ConsumeTrigger(&owner->actor)) {
        actor->animationState = 3;
        ActorVariation_SelectRandomPair(actor, &actor->movementTarget);
        actor->animationPhase = 0;
        {
            u32 state = 4;
            owner->actor.state = state;
        }
    }
    if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (SceneVolume_ConsumePendingEvent(actor)) {
        owner->actor.requestedVariant = owner->actor.variant;
        {
            u8 *request = &owner->actor.request;
            nextState = 1;
            *request = nextState;
        }
        owner->actor.requestPhase = 0;
        goto setState;
    } else if (owner->actor.pending) {
        actor->animationState = 3;
        nextState = 4;
    setState:
        owner->actor.state = nextState;
    }
}
