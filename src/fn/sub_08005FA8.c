#include "terrain_modifier.h"

s32 sub_08005FA8(u16 id)
{
    struct TerrainModifier *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        if (n->id == id)
            sub_08005F40(n);
        n = next;
    }
    return 0;
}
