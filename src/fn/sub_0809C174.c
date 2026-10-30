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
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = 0;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior,
                            u32 zero)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = zero;
    a->field2B1 = 1;
}
void Lun_DecayRouteMotion(struct SceneActorRegistryResetView *a)
{
    u8 *ctx = a->context;
    if (TakeFlag(&a->field2B1))
        SetControl(a, a->entryTarget224, 2);
    a->body.motion.x += *(s16 *)(ctx + 0x424);
    a->body.motion.z += *(s16 *)(ctx + 0x428);
    if (Mod(a->counter2AC, 4) == 0) {
        *(s16 *)(ctx + 0x424) >>= 1;
        *(s16 *)(ctx + 0x428) >>= 1;
    }
    if (a->field2B0) {
        u32 mask = 0x100;
        u16 *p = &a->flags2D4;
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
        a->counter2AC++;
}
