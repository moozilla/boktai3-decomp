#include "global.h"

struct Vec080A9688 {
    s16 x, y, z, w;
};
struct Entry080A9688 {
    u8 pad0[0x1c];
    struct Vec080A9688 pos;
    u8 pad24[0x104 - 0x24];
    u8 heading;
    u8 flags;
    u8 pad106[0x128 - 0x106];
};
struct Group080A9688 {
    u8 pad0[0x3d4];
    struct Entry080A9688 entries[4];
    u8 pad874[0x8da - 0x874];
    s16 count;
};
u8 sub_082215E4(s32, s32);

static inline u8 TestMask(u8 *flags, u32 mask)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

static inline void VecSub(struct Vec080A9688 *d, struct Vec080A9688 *a, struct Vec080A9688 *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

void sub_080A9688(u8 *p)
{
    struct Group080A9688 *g = *(struct Group080A9688 **)(p + 0x3d0);
    struct Vec080A9688 d;
    struct Vec080A9688 prev = *(struct Vec080A9688 *)(p + 0x5c);
    s32 i;
    for (i = 0; i < g->count; i++) {
        if (!TestMask(&g->entries[i].flags, 1)) {
            VecSub(&d, &prev, &g->entries[i].pos);
            g->entries[i].heading = sub_082215E4(d.x, d.z);
            prev = g->entries[i].pos;
        }
    }
}
