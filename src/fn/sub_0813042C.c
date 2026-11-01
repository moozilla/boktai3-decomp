#include "global.h"
#include "scene_actor_reaction.h"
#include "debug.h"

static inline u8 TakeReset(struct SceneActorRegistryResetView *actor)
{
    u8 *flag = &actor->field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline void Enter(struct SceneActorRegistryResetView *actor, u32 zero)
{
    u32 state = 10;
    u8 *field = &actor->state2A8;
    u32 offset;
    *field = zero;
    offset = 0x2A9;
    *((u8 *)actor + offset) = zero;
    offset++;
    *((u8 *)actor + offset) = state;
    offset += 2;
    *(u32 *)((u8 *)actor + offset) = zero;
    actor->field2B1 = 1;
}

void SceneActorControl_EnterTimedState(struct SceneActorRegistryResetView *actor)
{
    u32 masked;
    u16 flags;
    u32 zero;
    u32 bit;
    if (TakeReset(actor)) {
        DEBUG_PRINT();
    }
    bit = 0x20;
    masked = *(u16 *)((u8 *)actor + 0x2d4) & bit;
    flags = (u16)masked;
    zero = flags;
    if ((bit = zero) == 0)
        Enter(*(&actor), zero);
    actor->counter2AC++;
}
