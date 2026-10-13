#include "global.h"

struct S {
    u8 f[0xbe];
    u16 v;
};
u8 Scene_CanEnterNeighborCell(struct S *, u16, s32);

s32 sub_0800441C(struct S *s)
{
    s32 c = 0;
    s32 i = 0;
    do {
        if (Scene_CanEnterNeighborCell(s, s->v, i))
            c++;
        i++;
    } while (i <= 3);
    if (c > 2)
        return 1;
    return 0;
}
