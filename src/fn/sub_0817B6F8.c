#include "global.h"
#include "actor_state.h"
#define ActorState_UpdateDepletedState sub_0817B6F8
s32 sub_0817A23C(struct ActorState *);
s32 sub_0817AA7C(struct ActorState *);
void sub_0817B5C0(struct ActorState *, s32);
static inline u8 ConsumeTrigger(u8 *p)
{
    if (p[0xb0]) {
        p[0xb0] = 0;
        p[0xaf] = 0;
        return 1;
    }
    return 0;
}
void sub_0817B6F8(struct ActorStateOwner *owner)
{
    struct ActorState *p = &owner->actor;
    if (ConsumeTrigger((u8 *)&owner->actor)) {
        u32 state = 1;
        owner->actor.state = state;
    }
    if (sub_0817A23C(p))
        sub_0817B5C0(&owner->actor, 1);
    else if (sub_0817AA7C(&owner->actor))
        sub_0817B5C0(&owner->actor, 2);
    else if (owner->actor.timer > 60) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
    }
    owner->actor.timer++;
}
