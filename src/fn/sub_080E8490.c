#include "global.h"
#include "scene_actor_reaction.h"
static inline void clearFlag(u32 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}
static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}
static inline void resetState(struct SceneActorRegistryResetView *actor, u8 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void setControl(struct SceneActorRegistryResetView *actor, void *control, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    clearFlag(&actor->flags114);
    actor->word104 = 0;
}
static inline u32 testFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
void SceneActor_ResetCollisionReaction(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionMotion *motion;
    struct SceneActorRegistryControl *control;
    u16 *flags;
    resetState(actor, 0x11);
    setControl(actor, SceneActorControl_OnIdle, 0x10);
    motion = (void *)&actor->tracking.target;
    motion->fieldDC = 0;
    actor->field2C2 = 0x40;
    actor->group.flags414 = 0;
    actor->group.field418 = 0;
    flags = &actor->flags2D4;
    {
        u32 mask = ~0x380;
        mask &= *flags;
        *flags = mask;
    }
    {
        s32 mask = ~0x18;
        mask &= motion->flags;
        motion->flags = mask;
    }
    control = &actor->group.control;
    if (testFlags(actor, 0x200000)) {
        clearRouteFlag(&actor->flags2D0);
        {
            u32 mask = ~0x200;
            mask &= *flags;
            *flags = mask;
        }
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
    {
        u32 mask = ~0x400;
        mask &= *flags;
        *flags = mask;
    }
    actor->field5DA = 0xFF;
}
