#include "global.h"
#include "actor_state.h"
#define ActorState_UpdateSelectedAnimation sub_0817B8BC
#define ActorState_SelectAnimationPair ActorPosition_TestPath
s32 ActorMovement_CanReachTarget(struct ActorState *);
s32 SceneVolume_GetActivityState(struct ActorState *);
s32 SceneVolume_FindNearbyPosition(struct ActorState *);
s32 ActorPosition_TestPath(struct ActorState *, struct ActorAnimationPair *);
void SceneVolume_BeginPhase(struct ActorState *, s32);
s32 sub_0817B5E4(struct ActorState *, u32, s32, s32, s32);
static inline u8 ConsumeTrigger(struct ActorState *p)
{
    if (p->triggered) {
        p->triggered = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
void sub_0817B8BC(struct ActorStateOwner *owner)
{
    struct ActorState *p = &owner->actor;
    s32 result;
    if (ConsumeTrigger(&owner->actor)) {
        if (!ActorState_SelectAnimationPair(p, &p->selectedAnimation)) {
            owner->actor.requestedVariant = owner->actor.variant;
            owner->actor.request = 1;
            owner->actor.requestPhase = 0;
            owner->actor.state = 1;
            return;
        }
        p->animationState = 3;
        p->animation = p->selectedAnimation;
        {
            u32 state = 4;
            owner->actor.state = state;
        }
    }
    if (ActorMovement_CanReachTarget(p))
        SceneVolume_BeginPhase(&owner->actor, 1);
    else if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (owner->actor.timer > 60 && SceneVolume_FindNearbyPosition(p) == 1)
        SceneVolume_BeginPhase(&owner->actor, 3);
    if (p->requestMode == 2)
        result = sub_0817B5E4(&owner->actor, owner->actor.timer, 30, 10, 1);
    else
        result = sub_0817B5E4(&owner->actor, owner->actor.timer, 30, 10, 0);
    if (result) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
    }
    owner->actor.timer++;
}
