#include "global.h"
/* Skeleton_OnAttackCollision: records the hit unless the actor is unavailable */
static inline u32 Flag(u8 *a, u32 m, u32 off)
{
    return off == 0x2D4 ? *(u16 *)(a + off) & m : *(u32 *)(a + off) & m;
}
static inline u8 Unavailable(u8 *c)
{
    if (c && (Flag(c, 0x40, 0x2D4) || Flag(c, 0x20000, 0x2D0) || *(s16 *)(c + 0x9C) <= 0))
        return 1;
    return 0;
}

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

// confidence=low: records a collision target unless the Skeleton is unavailable.
void Skeleton_OnAttackCollision(u8 *unused, u8 *obj, u8 *ctx)
{
    int mask;
    u32 *flags;
    u8 *base = ctx;
    u32 *state = *(u32 **)(ctx + 0x3D0);
    if (Unavailable(ctx))
        return;
    {
        u32 bit = 0x10;
        u32 *sp = (u32 *)((u8 *)state + 0x6DC);
        u32 *held;
        u32 v;
        v = *sp & bit;
        held = sp;
        if (Test(v, ~0)) {
            u32 bit = 0x800;
            if (Test(*held, bit)) {
                *(u16 *)(base + 0x2EC) = 0;
                *(u16 *)(base + 0x2F0) = 0;
            }
            return;
        }
    }
    if ((Unavailable(ctx) == 0) && ((*((u16 *)(obj + 0xA))) & 0xF)) {
        *((u32 *)(ctx + 0x304)) = *((u16 *)(obj + 4));
        flags = (u32 *)(ctx + 0x2D0);
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}
