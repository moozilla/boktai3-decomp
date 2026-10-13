#include "terrain_modifier.h"

void sub_0821D6D0(void *);
void sub_08219D38(void *);

u32 sub_08005F40(struct TerrainModifier *n)
{
    if (n->active != 0) {
        sub_0821D6D0(n->registration);
        n->active = 0;
    }
    if (gUnk_02000028 != 0)
        sub_08005E40(n);
    sub_08219D38(n);
    return 0;
}
