#include "global.h"

struct Q { u8 filler[0xec8]; s8 v[3]; };
struct S080FB248 { u8 filler[0x3d0]; struct Q *q; };

u32 sub_080FB248(struct S080FB248 *s, s32 x)
{
    struct Q *q = s->q;
    s32 i;
    for (i = 0; i < 3; i++) {
        if (q->v[i] == x)
            return 1;
    }
    return 0;
}
