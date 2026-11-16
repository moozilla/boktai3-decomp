#include "global.h"
#include "scene_actor_reaction.h"

static inline u8 TakeFlag2B1(struct ChaseActor *actor)
{
    if (actor->base.field2B1) {
        actor->base.field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}

void SceneActorControl_BeginIdleControl(struct ChaseActor *actor)
{
    if (TakeFlag2B1(actor)) {
        void *target = SceneActorControl_OnIdle;
        SetControl(actor, 0x10, target);
    }
    actor->base.counter2AC++;
}
