#include "global.h"
#define TentacleActor_TargetMarkedSegment sub_080A97AC
struct Pos080A97AC {
    u32 v[2];
};
struct Entry080A97AC {
    u8 pad0[0x1c];
    struct Pos080A97AC pos;
    u8 pad24[0x105 - 0x24];
    u8 flags;
    u8 pad106[0x128 - 0x106];
};
struct Group080A97AC {
    u8 pad0[0x3d4];
    struct Entry080A97AC entries[4];
    u8 pad874[0x894 - 0x874];
    struct Pos080A97AC savedPos;
    u8 pad89c[0x8b4 - 0x89c];
    struct Entry080A97AC *target;
    u8 pad8b8[0x8d0 - 0x8b8];
    u16 state;
    u8 pad8d2[0x8da - 0x8d2];
    s16 count;
};
inline static u8 TestMask(struct Entry080A97AC *e, char mask)
{
    if (e->flags & mask)
        return 1;
    return 0;
}
inline static u8 TestMask16(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}

void sub_080A97AC(u8 *p)
{
    struct Group080A97AC *g = *(struct Group080A97AC **)(p + 0x3d0);
    s32 found = 0xff;
    s32 i;
    for (i = 0; i < g->count; i++) {
        if (TestMask(&g->entries[i], 4)) {
            found = i;
            break;
        }
    }
    if (found != 0xff) {
        u32 mask = 1;
        if (TestMask16(&g->state, mask)) {
            g->savedPos = g->entries[found].pos;
            g->target = &g->entries[found];
        }
    }
}
