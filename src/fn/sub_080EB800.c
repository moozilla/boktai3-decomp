#include "global.h"
#include "scene_actor_reset.h"

void Octopus_UpdateHold(void);

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}

static inline u8 TakeReset(struct SceneActorRegistryResetView *actor)
{
    u8 *flag = &actor->base.field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline void SetStateB(struct SceneActorRegistryResetView *actor)
{
    u32 two = 2;
    u32 one = 1;
    u8 *field = &actor->base.state2A8;
    u32 zero = 0;
    *field = zero;
    actor->base.phase2A9 = two;
    actor->base.behavior2AA = one;
    actor->base.counter2AC = zero;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = 1;
    }
}

void Octopus_StartHoldControl(struct SceneActorRegistryResetView *actor)
{
    TakeReset(actor);
    if (actor->base.field2B0) {
        SetControl(actor, Octopus_UpdateHold, 0x25);
        SetStateB(actor);
    } else
        actor->base.counter2AC++;
}
