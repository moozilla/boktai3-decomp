#include "terrain_modifier.h"

u32 sub_08005E40(struct TerrainModifier *p)
{
    struct TerrainModifier *prev = p->previous;
    struct TerrainModifier *next = p->next;
    if (prev) {
        prev->next = next;
    } else {
        gUnk_02000028->head = next;
    }
    if (next) {
        next->previous = prev;
    }
    return 0;
}
