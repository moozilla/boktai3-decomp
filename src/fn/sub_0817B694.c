#include "global.h"
#define ActorState_BeginDepletedState sub_0817B694
#include "actor_state.h"
s32 sub_0817B694(struct ActorState *p)
{
    s32 result;
    p->checking = 1;
    if (p->vitality <= 0) {
        u32 mask = 2;
        p->flags |= mask;
        {
            u32 state = 13;
            p->state = state;
        }
    }
    p->checking = 0;
    {
        u32 mask = 2;
        if (!(p->flags & mask))
            result = 0;
        else {
            u32 variant = 3;
            p->active = 1;
            p->phase = 0;
            p->variant = variant;
            p->timer = 0;
            p->triggered = 1;
            result = 1;
        }
    }
    return result;
}
