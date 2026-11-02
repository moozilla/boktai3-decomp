#include "global.h"
#include "actor_state.h"
#include "actor_spatial_query.h"

union PulseActor {
    struct ActorStateOwner owner;
    struct MovementReachActor movement;
    struct SceneVolumeSearch volume;
};

s32 SceneVolume_GetActivityState(struct ActorState *);
void SceneVolume_BeginPhase(struct ActorState *, s32);
s32 ActorEffect_StepTurn(struct ActorState *, u32, s32, s32, s32);

static inline s32 GetPulseTrigger(struct ActorStateOwner *owner, u8 **trigger)
{
    *trigger = &owner->actor.triggered;
    return **trigger;
}

void ActorDecision_UpdatePulsePhase(union PulseActor *input)
{
    struct ActorStateOwner *owner = &input->owner;
    union PulseActor *actor;
    u8 *trigger;
    s32 result;
    actor = input;
    if (GetPulseTrigger(owner, &trigger)) {
        actor = (union PulseActor *)(trigger - ((u8 *)&owner->actor.triggered - (u8 *)owner));
        actor->owner.actor.triggered = 0;
        owner->actor.pending = 0;
    }
    if (ActorMovement_CanReachTarget(&actor->movement))
        SceneVolume_BeginPhase(&owner->actor, 1);
    else if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (owner->actor.timer > 60 && SceneVolume_FindNearbyPosition(&actor->volume) == 1)
        SceneVolume_BeginPhase(&owner->actor, 3);
    else if (owner->actor.timer > 210) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
    }
    if (actor->owner.actor.requestMode == 2)
        result = ActorEffect_StepTurn(&owner->actor, owner->actor.timer, 30, 30, 1);
    else
        result = ActorEffect_StepTurn(&owner->actor, owner->actor.timer, 30, 30, 0);
    if (result) {
        u32 phase = 6;
        owner->actor.state = phase;
    }
    owner->actor.timer++;
}
