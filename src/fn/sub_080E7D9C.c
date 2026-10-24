#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"

static inline void Mimic_ClearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}
static inline u32 Mimic_TestFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

// confidence=low: drops the acquired target when its response cooldown expires.
void Mimic_ResetExpiredTarget(struct SceneActorRegistryResetView *actor)
{
    u16 *flags;
    u16 *timer;
    u32 activeMask = 0x80;
    flags = &actor->flags2D4;
    if (!(activeMask & *flags))
        return;
    timer = &actor->group.field418;
    if (!*timer && !(*flags & 0x38)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        motion = (void *)&actor->fieldDC;
        motion->fieldDC = 0;
        actor->field2C2 = 0x40;
        actor->group.flags414 = 0;
        *timer = 0;
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
        if (Mimic_TestFlags(actor, 0x200000)) {
            Mimic_ClearRouteFlag(&actor->flags2D0);
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
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 16);
        Chase_SetState(actor, 0, 0, 17);
    }
}
