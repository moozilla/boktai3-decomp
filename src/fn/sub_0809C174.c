#include "global.h"
#include "scene_actor_reset.h"

// confidence=low: decays saved horizontal route motion before returning to pursuit or rest.

s32 Mod(s32, s32);
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 kind)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = 0;
    a->base.field2B5 = kind;
    a->base.target = callback;
    a->base.field2C8 = 0;
    {
        u32 *flags = &a->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->base.word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior,
                            u32 zero)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}
void Lun_DecayRouteMotion(struct SceneActorRegistryResetView *a)
{
    u8 *ctx = a->base.context;
    if (TakeFlag(&a->base.field2B1))
        SetControl(a, a->base.entryTarget224, 2);
    a->base.body.motion.x += *(s16 *)(ctx + 0x424);
    a->base.body.motion.z += *(s16 *)(ctx + 0x428);
    if (Mod(a->base.counter2AC, 4) == 0) {
        *(s16 *)(ctx + 0x424) >>= 1;
        *(s16 *)(ctx + 0x428) >>= 1;
    }
    if (a->base.field2B0) {
        u32 mask = 0x100;
        u16 *p = &a->base.flags2D4;
        u16 flags;
        {
            u32 raw = *p & mask;
            flags = (u16)raw;
        }
        if (flags)
            SetState(a, 2, 1, 0);
        else
            SetState(a, 0, 2, flags);
    } else
        a->base.counter2AC++;
}
