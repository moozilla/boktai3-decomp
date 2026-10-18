#include "global.h"
#include "actor_state.h"
#define ActorState_UpdateRandomAnimation sub_0817B788
#define gActorAnimationRandomIndex       gUnk_03005308
#define gActorAnimationRandomValues      gUnk_0203B400
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
s32 sub_0817A23C(struct ActorState *);
s32 sub_0817AA7C(struct ActorState *);
s32 sub_0817AA14(struct ActorState *);
void sub_0817B5C0(struct ActorState *, s32);
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
void sub_0817B788(struct ActorStateOwner *owner)
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
    if (sub_0817A23C(p))
        sub_0817B5C0(&owner->actor, 1);
    else if (sub_0817AA7C(&owner->actor))
        sub_0817B5C0(&owner->actor, 2);
    else if (sub_0817AA14(p) == 1)
        sub_0817B5C0(&owner->actor, 3);
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
