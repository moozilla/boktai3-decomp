#include "global.h"
/* Skeleton_OnContactEvent: records the hit and flags the state unless the actor is unavailable */
inline static u8 blocked2048(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

#ifdef NONMATCHING
// fuzzy: 98.4%
// Register allocation differs for the saved extended-state flags pointer.
#define Skeleton_OnAttackCollision sub_080B8DE0
void Skeleton_OnAttackCollision(u8 *unused, u8 *obj, u8 *ctx)
{
    int mask;
    u32 *flags;
    u8 *base = ctx;
    u32 *state = *(u32 **)(ctx + 0x3D0);
    if (blocked2048(ctx))
        return;
    {
        u32 bit = 0x10;
        u32 *sp = (u32 *)((u8 *)state + 0x6DC);
        u32 *held;
        u32 v = *sp & bit;
        held = sp;
        state = held;
        if (Test(v, ~0)) {
            u32 bit = 0x800;
            if (Test(*state, bit)) {
                *(u16 *)(base + 0x2EC) = 0;
                *(u16 *)(base + 0x2F0) = 0;
            }
            return;
        }
    }
    if ((blocked2048(ctx) == 0) && ((*((u16 *)(obj + 0xA))) & 0xF)) {
        *((u32 *)(ctx + 0x304)) = *((u16 *)(obj + 4));
        flags = (u32 *)(ctx + 0x2D0);
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B8DE0);
#endif
