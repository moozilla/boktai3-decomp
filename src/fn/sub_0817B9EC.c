#include "global.h"
#include "actor_state.h"
void sub_0817A034(struct ActorState *, struct ActorAnimationPair *, s32);
s32 SceneVolume_ConsumePendingEvent(struct ActorState *);
static inline u8 ConsumeTrigger(struct ActorState *p)
{
    if (p->triggered) {
        p->triggered = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
void ActorState_BeginReverseAnimation(struct ActorStateOwner *owner)
{
    struct ActorState *p = &owner->actor;
    if (ConsumeTrigger(&owner->actor)) {
        p->animationState = 3;
        sub_0817A034(p, &p->animation, (p->direction + 128) & 255);
        {
            u32 state = 4;
            owner->actor.state = state;
        }
    }
    if (SceneVolume_ConsumePendingEvent(p)) {
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
