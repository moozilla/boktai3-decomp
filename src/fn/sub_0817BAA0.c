#include "global.h"
#include "actor_state.h"
#define ActorState_UpdateRestartableAnimation sub_0817BAA0
void ActorVariation_SelectRandomPair(struct ActorState *, struct ActorAnimationPair *);
s32 SceneVolume_ConsumePendingEvent(struct ActorState *);
s32 SceneVolume_GetActivityState(struct ActorState *);
void SceneVolume_BeginPhase(struct ActorState *, s32);
static inline u8 ConsumeTrigger(struct ActorState *p)
{
    if (p->triggered) {
        p->triggered = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
void sub_0817BAA0(struct ActorStateOwner *owner)
{
    struct ActorState *p = &owner->actor;
    u32 nextState;
    if (ConsumeTrigger(&owner->actor)) {
        p->animationState = 3;
        ActorVariation_SelectRandomPair(p, &p->animation);
        p->animationPhase = 0;
        {
            u32 state = 4;
            owner->actor.state = state;
        }
    }
    if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (SceneVolume_ConsumePendingEvent(p)) {
        owner->actor.requestedVariant = owner->actor.variant;
        {
            u8 *request = &owner->actor.request;
            nextState = 1;
            *request = nextState;
        }
        owner->actor.requestPhase = 0;
        goto setState;
    } else if (owner->actor.pending) {
        p->animationState = 3;
        nextState = 4;
    setState:
        owner->actor.state = nextState;
    }
}
