#include "global.h"

struct Entry080A8CDC {
    u8 pad0[0x1c];
    u32 pos[2];
    u8 pad24[0x128 - 0x24];
};
struct Group080A8CDC {
    u8 pad0[0x3d4];
    struct Entry080A8CDC entries[4];
    u8 pad874[0x8d3 - 0x874];
    u8 delay;
    u8 pad8d4[0x8da - 0x8d4];
    s16 count;
};
void sub_080ACCD4(void *, s32, void *);

void sub_080A8CDC(u8 *p, void *pos)
{
    struct Group080A8CDC *g = *(struct Group080A8CDC **)(p + 0x3d0);
    if (g->delay) {
        g->delay--;
        return;
    }
    g->delay = 12;
    sub_080ACCD4(g, g->count - 1, g->entries[g->count >> 1].pos);
    sub_080ACCD4(g, g->count >> 1, g->entries[0].pos);
    sub_080ACCD4(g, 0, pos);
}
