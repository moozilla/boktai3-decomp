#include "global.h"
#include "scene_actor_control_phase.h"

#ifdef NONMATCHING
// fuzzy: 0%
// Remaining differences: register allocation, address reuse, and instruction order.
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))

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

static inline u32 Actor16(struct SceneActorRegistryResetView *a, u32 m)
{
    return a->flags2D4 & m;
}
static inline u32 Actor32(struct SceneActorRegistryResetView *a, u32 m)
{
    return a->flags2D0 & m;
}
static inline u8 Disabled(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Actor16(a, 64))
            return 1;
        if (Actor32(a, 0x20000))
            return 1;
        if (F(a, 0x9c, s16) <= 0)
            return 1;
    }
    return 0;
}
struct ContactPosition *TetherActor_GetPosition(void);
s32 sub_0821F204(void *, void *, void *);
s32 sub_0821F7A8(void *, void *, void *);
static inline u8 Route(struct SceneActorRegistryResetView *a, void *nav, void *pos, void *target)
{
    u16 *flags;
    s32 r = sub_0821F204(nav, pos, target);
    if (!r) {
        u16 off = 0x2d4;
        flags = (void *)((u8 *)a + off);
        goto clear;
    }
    r = sub_0821F7A8(nav, pos, target);
    if (!r) {
        u32 off = 0x2d4;
        flags = (void *)((u8 *)a + off);
        goto clear;
    }
    if (r != 2)
        Memory_Clear((u8 *)a + 0x64, 8);
    return 1;
clear: {
    u32 m = 0xfffffdff;
    u32 v = *flags;
    m &= v;
    *flags = m;
}
    return 0;
}
#define SceneActorPlacement_TriggerTetherAction sub_080A3CD8
s32 SceneActorPlacement_TriggerTetherAction(struct SceneActorRegistryResetView *a)
{
    u8 *c = (void *)SceneActorControl_GetState(a);
    u8 *nav = (u8 *)a + 0x3d4;
    struct ContactPosition *target;
    if (Disabled(a))
        return 0;
    target = TetherActor_GetPosition();
    if (!target)
        return 0;
    if (!Route(a, F(nav, 0x1fc, void *), &a->position, target))
        return 0;
    {
        u16 *p = (void *)(c + 0x62c);
        u32 v = *p;
        u32 m = 4;
        m |= v;
        *p = m;
    }
    Control(a, F(a, 0x228, void *), 1);
    State(a, 7);
    a->field2C2 = 16;
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorPlacement_TriggerTetherAction);
#endif
