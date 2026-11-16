#include "global.h"

struct QIsRoutePointRecent {
    u8 filler[0xec8];
    s8 v[3];
};
struct S080FB248 {
    u8 filler[0x3d0];
    struct QIsRoutePointRecent *q;
};

u32 Axe_IsRoutePointRecent(struct S080FB248 *s, s32 x)
{
    struct QIsRoutePointRecent *q = s->q;
    s32 i;
    for (i = 0; i < 3; i++) {
        if (q->v[i] == x)
            return 1;
    }
    return 0;
}
