// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"

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

#define SceneActorControl_EnterTimedState sub_0813042C

void SceneActorControl_EnterTimedState(struct SceneActorRegistryResetView *actor)
{
    register u32 bit asm("r1");
    u32 zero;
    if (TakeReset(actor)) {
    }
    bit = 0x20;
    zero = (u16)(*(u16 *)((u8 *)actor + 0x2d4) & bit);
    if (zero == 0)
        Enter(actor, zero);
    actor->counter2AC++;
}
