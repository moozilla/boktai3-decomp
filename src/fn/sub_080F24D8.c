#include "global.h"

u32 Terrain_GetHeightAt(void *);

struct S {
    u8 filler[0xb84];
    u8 a[0xd00 - 0xb84];
    u32 v;
    u8 f2[0xd80 - 0xd04];
    s32 n;
};

void Ivy_UpdateGroundHeight(struct S *s)
{
    if (s->n > 7)
        s->v = (u16)Terrain_GetHeightAt(s->a);
}
