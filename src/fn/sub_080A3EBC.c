#include "global.h"
#include "scene_actor_control_phase.h"
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))
void SceneActorMovement_ResolveRoutePosition(void *);

static inline void Control(struct SceneActorRegistryResetView *a, void *c, u32 k)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = k;
    a->target = c;
    a->field2C8 = 0;
    {
        u32 *p = &a->flags114;
        u32 m = ~1;
        *p &= m;
    }
    a->word104 = 0;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 k)
{
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = k;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

static inline u8 SaveControlFlags(u8 *c, u16 **out, u32 m)
{
    u16 *p = (void *)(c + 0x62c);
    u16 value = *p;
    value &= m;
    *out = p;
    if (value)
        return 1;
    return 0;
}
static inline u16 Actor16(struct SceneActorRegistryResetView *a, u32 m)
{
    u16 value = a->flags2D4;
    value &= m;
    return value;
}
static inline u32 Actor32(struct SceneActorRegistryResetView *a, u32 m)
{
    return a->flags2D0 & m;
}
static inline u8 Disabled(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Actor16(a, 64) || Actor32(a, 0x20000) || a->health <= 0)
            return 1;
    }
    return 0;
}

s32 Div(s32, s32);
static inline u8 Flag16(u16 *p, u32 m)
{
    if (*p & m)
        return 1;
    return 0;
}
static inline u8 Slow(void)
{
    u32 m = 32;
    if (F(gGameState, 0x868, u32) & m)
        return 1;
    return 0;
}
// confidence=low: global flag applies a temporary movement slowdown
void SceneActorPlacement_UpdateMovementSlowdown(struct SceneActorRegistryResetView *a)
{
    u8 *c = (void *)SceneActorControl_GetState(a);
    u16 *flags;
    if (Disabled(a))
        return;
    if (SaveControlFlags(c, &flags, 16)) {
        if (Slow()) {
            if (!Flag16(flags, 8)) {
                SceneActorMovement_ResolveRoutePosition(a);
                State(a, 7);
                Control(a, F(a, 0x228, void *), 1);
                {
                    u32 v = *flags;
                    u32 m = 8;
                    m |= v;
                    *flags = m;
                }
            }
            a->field2C2 = Div(64, 5);
        } else if (Flag16(flags, 8)) {
            u16 *p = flags;
            u32 m = ~8;
            *p &= m;
            a->field2C2 = 64;
        }
    }
}
