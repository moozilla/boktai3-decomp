#include "global.h"
#include "scene_actor_reaction.h"

void SceneActorControl_OnIdle(void);

static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}

#define SceneActorControl_BeginIdleControl sub_081305F8

void SceneActorControl_BeginIdleControl(struct SceneActorRegistryResetView *actor)
{
    if (TakeFlag2B1(actor)) {
        void *target = SceneActorControl_OnIdle;
        SetControl(actor, 0x10, target);
    }
    actor->counter2AC++;
}
