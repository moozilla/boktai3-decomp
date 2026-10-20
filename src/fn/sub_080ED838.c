#include "global.h"
#include "scene_actor_reaction.h"

void Ivy_UpdateActiveState(void);

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}

static inline void SetStateB(struct SceneActorRegistryResetView *actor)
{
    u32 two = 2;
    u8 *field = &actor->state2A8;
    u32 zero = 0;
    *field = zero;
    actor->phase2A9 = zero;
    actor->behavior2AA = two;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}

void SceneActorReaction_FinishAnimation17(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *destination = &actor->colliderA.flags;
        u32 flags = *destination;
        u32 mask = 0xFFFB;
        mask &= flags;
        mask |= 8;
        *destination = mask;
        {
            u16 *p = &actor->flags2C0;
            u32 mask = 0xFFFFF7FF;
            *p = mask & *p;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 17);
    }
    if (actor->field2B3) {
        SetControl(actor, Ivy_UpdateActiveState, 0x22);
        SetStateB(actor);
    }
}
