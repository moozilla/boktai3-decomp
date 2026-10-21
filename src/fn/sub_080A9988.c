#include "global.h"
#define TentacleActor_ShiftSegmentTrail sub_080A9988

struct Pos080A9988 {
    u32 v[2];
};
struct Entry080A9988 {
    u8 pad0[0x1c];
    struct Pos080A9988 pos;
    u8 pad24[0x105 - 0x24];
    u8 flags;
    u8 pad106[0x114 - 0x106];
    struct Pos080A9988 prev;
    u8 pad11C[0x128 - 0x11C];
};
struct Group080A9988 {
    u8 pad0[0x3d4];
    struct Entry080A9988 entries[4];
};
struct Body080A9988 {
    u8 pad0[4];
    u8 *obj;
    u8 pad8[0x2ca - 8];
    u16 f2CA;
    u8 pad2CC[0x3d0 - 0x2cc];
    struct Group080A9988 *g;
};
s32 Mod(s32, s32);

inline static u8 blocked(u8 *c)
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
inline static u8 TestMask(struct Entry080A9988 *e, char mask)
{
    if (e->flags & mask)
        return 1;
    return 0;
}

void TentacleActor_ShiftSegmentTrail(struct Body080A9988 *p)
{
    struct Group080A9988 *g;
    struct Pos080A9988 carry;
    s32 i;
    if (blocked((u8 *)p))
        return;
    g = p->g;
    if (Mod(p->f2CA, *((u8 *)g + 0x874)))
        return;
    carry = *(struct Pos080A9988 *)(p->obj + 0x1c);
    for (i = 0; i < *(s16 *)((u8 *)g + 0x8da); i++) {
        if (TestMask(&g->entries[i], 2) && !TestMask(&g->entries[i], 1)) {
            struct Pos080A9988 t = g->entries[i].pos;
            g->entries[i].prev = carry;
            carry = t;
        }
    }
}
