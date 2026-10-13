#include "terrain_modifier.h"

void sub_0821D6D0(void *);

s32 sub_08005FE4(u16 id)
{
    struct TerrainModifier *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        if (n->id == id && n->active != 0) {
            n->active = 0;
            sub_0821D6D0(n->registration);
        }
        n = next;
    }
    return 0;
}
