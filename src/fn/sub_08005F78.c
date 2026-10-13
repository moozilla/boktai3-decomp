#include "terrain_modifier.h"

s32 sub_08005F78(void)
{
    struct TerrainModifier *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        sub_08005F40(n);
        n = next;
    }
    return 0;
}
