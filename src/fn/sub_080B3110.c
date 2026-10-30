#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 79.8%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: reset scheduling, saved zero values, and group-address lifetimes.
void sub_080B1FA8(void *);
void sub_080ADB90(void);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
static inline u8 Test(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline void ResetContext(struct SceneActorRegistryResetView *a)
{
    struct SceneActorReactionMotion *motion;
    struct SceneActorRegistryControl *control;
    u16 *flags;
    a->field2C2 = 64;
    a->group.flags414 = 0;
    a->group.field418 = 0;
    flags = &a->flags2D4;
    {
        s32 mask = ~0x380;
        *flags = mask & *flags;
    }
    motion = (void *)&a->tracking.target;
    {
        s32 mask = ~0x18;
        motion->flags = mask & motion->flags;
    }
    control = &a->group.control;
    if (Chase_Bits32(&a->flags2CC, 0x200000)) {
        u32 *state = &a->flags2D0;
        s32 mask = ~0x1000000;
        *state = mask & *state;
        {
            s32 mask = ~0x200;
            *flags = mask & *flags;
        }
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
    {
        s32 mask = ~0x400;
        *flags = mask & *flags;
    }
    a->field5DA = 255;
}
// confidence=low: prepares context response state and starts pursuit.
#define Vorn_BeginResponsePursuit sub_080B3110
static inline u8 Test_1(u16 *p)
{
    if ((*p) & 1)
        return 1;
    return 0;
}

inline static void ResetContext_080B3110(struct SceneActorRegistryResetView *a)
{
    struct SceneActorReactionMotion *motion;
    struct SceneActorRegistryControl *control;
    u16 *flags;
    s32 t1;
    a->field2C2 = 64;
    a->group.flags414 = 0;
    a->group.field418 = 0;
    flags = &a->flags2D4;
    {
        s32 mask = ~0x380;
        *flags = mask & (*flags);
    }
    motion = (void *)(&a->tracking.target);
    {
        s32 mask = ~0x18;
        motion->flags = mask & motion->flags;
    }
    control = &a->group.control;
    t1 = 0x200000;
    if (Chase_Bits32(&a->flags2CC, t1)) {
        u32 *state = &a->flags2D0;
        s32 mask = ~0x1000000;
        *state = mask & (*state);
        {
            s32 mask = ~0x200;
            *flags = mask & (*flags);
        }
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
    {
        s32 mask = ~0x400;
        *flags = mask & (*flags);
    }
    a->field5DA = 255;
}

inline static u8 Test_1_080B3110(u16 *p)
{
    {
        u16 k1 = 1;
        if ((*p) & k1)
            return 1;
    }
    return 0;
}

void Vorn_BeginResponsePursuit(struct SceneActorRegistryResetView *a, u32 reset, const void *unused,
                               u32 unused2)
{
    struct SceneActorRegistryResetView *ctx = (void *)a->context;
    u8 *group = ((u8 *)ctx) + 0x3D4;
    u16 *flags = &ctx->flags6D4;
    typeof(*((u16 *)(((u8 *)ctx) + 0x69A))) t1;
    if (ctx->field69E)
        return;
    if (Test_1_080B3110(flags) && (!Test(flags, 2)))
        return;
    ctx->field69E = 60;
    ResetContext_080B3110(ctx);
    {
        u32 value = 1 | (*flags);
        value |= 0;
        value &= ~2;
        *flags = value;
    }
    t1 = *((u16 *)(((u8 *)ctx) + 0x69A));
    *((u16 *)(((u8 *)ctx) + 0x698)) = t1;
    a->body.direction = a->field2B6;
    if (reset == 1)
        SceneBody_StartRoute(&a->body.id, group + 0x74, group[0x81 + (group[0x83] ^ 1)], 0, 0);
    sub_080B1FA8(a);
    {
        u32 mask = 4;
        a->flags2D0 = mask | a->flags2D0;
    }
    {
        s8 mask = ~16;
        a->flags2CC = mask & a->flags2CC;
    }
    Chase_SetBehavior(a, 15);
    Chase_SetCtl(a, *((void **)(((u8 *)a) + 0x22C)), 0);
    a->word104 = (u32)sub_080ADB90;
    a->word108 = 0;
    a->word10C = 0;
    a->word110 = 35;
    a->flags114 = 1;
    {
        struct SceneActorReactionMotion *m = (void *)(&a->tracking.target);
        u32 mask = 8;
        m->flags = mask | m->flags;
    }
    {
        u32 mask = 128;
        a->flags2CC = mask | a->flags2CC;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080B3110);
#endif
