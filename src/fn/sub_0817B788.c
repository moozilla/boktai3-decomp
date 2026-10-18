#include "global.h"
#include "actor_state.h"
#define gActorAnimationRandomIndex  gUnk_03005308
#define gActorAnimationRandomValues gUnk_0203B400
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
s32 ActorMovement_CanReachTarget(struct ActorState *);
s32 SceneVolume_GetActivityState(struct ActorState *);
s32 SceneVolume_FindNearbyPosition(struct ActorState *);
void SceneVolume_BeginPhase(struct ActorState *, s32);
s32 sub_0817B5E4(struct ActorState *, u32, s32, s32, s32);
s32 sub_0821E920(void *, u32);
static inline u8 ConsumeTrigger(struct ActorState *p)
{
    if (p->triggered) {
        p->triggered = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
static inline s32 RandomValue(void)
{
    u32 offset;
    gActorAnimationRandomIndex = (gActorAnimationRandomIndex + 1) & 0x3ff;
    offset = gActorAnimationRandomIndex * 2;
    return *(u16 *)(offset + (u32)gActorAnimationRandomValues);
}
void ActorState_UpdateRandomAnimation(struct ActorStateOwner *owner)
{
    struct ActorState *p = &owner->actor;
    s32 duration;
    s32 result;
    if (ConsumeTrigger(p)) {
        p->animationIndex = (RandomValue() >> 8) & 3;
        if (sub_0821E920(&p->animation, p->animationIndex) >= 0)
            p->animationState = 3;
        {
            u32 state = 4;
            owner->actor.state = state;
        }
    }
    duration = p->mode == 1 ? 20 : 60;
    if (ActorMovement_CanReachTarget(p))
        SceneVolume_BeginPhase(&owner->actor, 1);
    else if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (SceneVolume_FindNearbyPosition(p) == 1)
        SceneVolume_BeginPhase(&owner->actor, 3);
    if (p->requestMode == 2)
        result = sub_0817B5E4(&owner->actor, owner->actor.timer, duration, duration, 1);
    else
        result = sub_0817B5E4(&owner->actor, owner->actor.timer, duration, duration, 0);
    if (result) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
    }
    owner->actor.timer++;
}
