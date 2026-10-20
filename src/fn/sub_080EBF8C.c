#include "global.h"
#include "scene_actor_reset.h"
void SceneActorAnimation_SelectDirectionalFlags(void *, u32);
void SceneActorControl_OnIdle(void);
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
void Octopus_BeginIdleAnimation(struct SceneActorRegistryResetView *actor)
{
    u32 flag;
    SceneActorAnimation_SelectDirectionalFlags(actor, 0x10);
    flag = 1;
    if (actor->drawableKind == 0) {
        u32 *flags = actor->drawable;
        *flags |= flag;
    } else {
        u32 *flags = (u32 *)((u8 *)actor->drawable + 0x20);
        flags[2] |= flag;
    }
    {
        u32 one = 1;
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = one;
        actor->counter2AC = 0;
        actor->field2B1 = one;
    }
    SetControl(actor, SceneActorControl_OnIdle, 0x10);
}
