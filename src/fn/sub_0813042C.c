#include "global.h"
#include "scene_actor_reaction.h"
#include "debug.h"

static inline u8 TakeReset(struct ChaseActor *actor)
{
    u8 *flag = &actor->base.field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline void Enter(struct ChaseActor *actor, u32 zero)
{
    u32 state = 10;
    u8 *field = &actor->base.state2A8;
    u32 offset;
    *field = zero;
    offset = 0x2A9;
    *((u8 *)actor + offset) = zero;
    offset++;
    *((u8 *)actor + offset) = state;
    offset += 2;
    *(u32 *)((u8 *)actor + offset) = zero;
    actor->base.field2B1 = 1;
}

void SceneActorControl_EnterTimedState(struct ChaseActor *actor)
{
    u32 masked;
    u16 flags;
    u32 zero;
    u32 bit;
    if (TakeReset(actor)) {
        DEBUG_PRINT();
    }
    bit = 0x20;
    masked = actor->base.flags2D4 & bit;
    flags = (u16)masked;
    zero = flags;
    if ((bit = zero) == 0)
        Enter(actor, zero);
    actor->base.counter2AC++;
}
