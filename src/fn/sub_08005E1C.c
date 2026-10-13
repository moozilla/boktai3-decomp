#include "terrain_modifier.h"

u32 sub_08005E1C(struct TerrainModifier *p)
{
    struct TerrainModifier *t;
    p->previous = 0;
    t = gUnk_02000028->head;
    p->next = t;
    if (t) {
        t->previous = p;
    }
    gUnk_02000028->head = p;
    return 0;
}
