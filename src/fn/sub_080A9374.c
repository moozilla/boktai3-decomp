#include "global.h"

struct Entry080A9374 {
    u8 pad0[0x105];
    u8 flags;
    u8 pad106[0x128 - 0x106];
};
struct Group080A9374 {
    u8 pad0[0x3d4];
    struct Entry080A9374 entries[4];
    u8 pad874[0x8da - 0x874];
    s16 count;
};
void sub_082212DC(void *, void *, u32, u32);

static inline u8 TestMask(u8 *flags, u32 mask)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

void sub_080A9374(u8 *p, void *arg)
{
    struct Group080A9374 *g = *(struct Group080A9374 **)(p + 0x3d0);
    struct Entry080A9374 *e = g->entries;
    s32 i;
    for (i = 0; i < g->count; i++) {
        if (!TestMask(&e[i].flags, 1))
            sub_082212DC(&e[i], arg, 10, 30);
    }
}
