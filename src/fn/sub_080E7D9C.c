// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"

static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}
static inline u32 testFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

static inline u16 TargetFlags(struct SceneActorRegistryResetView *actor, u32 mask, u16 **out)
{
    *out = &actor->flags2D4;
    return **out & mask;
}
#ifdef NONMATCHING
// fuzzy: 89.8%
// Remaining differences: retained flags pointer, zero scheduling and control offset reuse.
// confidence=low: drops the acquired target when its response cooldown expires.
void Mimic_ResetExpiredTarget(struct SceneActorRegistryResetView *actor)
{
    u16 *flags;
    u16 *timer;
    if (!TargetFlags(actor, 0x80, &flags))
        return;
    timer = (void *)((u8 *)actor + 0x418);
    if (!*timer && !(*flags & 0x38)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        motion = (void *)&actor->fieldDC;
        motion->fieldDC = 0;
        {
            register u32 o asm("r0") = 0x2C2;
            register u16 *h asm("r1");
            register u32 v asm("r0");
            asm("" : "+r"(o));
            h = (u16 *)((u8 *)actor + o);
            v = 0x40;
            asm("" : "+r"(v));
            *h = v;
        }
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
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 16);
        Chase_SetState(actor, 0, 0, 17);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Mimic_ResetExpiredTarget);
#endif
