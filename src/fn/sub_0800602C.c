#include "terrain_modifier.h"

void sub_0821D698(u8 *, u32, u32, u32, u32, u32);
s32 sub_0800602C(u32 id)
{
    struct TerrainModifier *e;
    struct TerrainModifier *n;
    u16 k = id;
    if (gUnk_02000028 == 0)
        return -1;
    e = gUnk_02000028->head;
    while (e != 0) {
        n = e->next;
        if (e->id == k && e->active == 0) {
            e->active = 1;
            sub_0821D698(e->registration, e->cell, e->tile, e->height, 0xff, e->flags);
        }
        e = n;
    }
    return 0;
}
