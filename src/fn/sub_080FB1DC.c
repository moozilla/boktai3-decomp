#include "global.h"

struct QInitRoutePointHistory {
    u8 filler[0xec8];
    u8 v[3];
    u8 w;
};
struct S080FB1DC {
    u8 filler[0x3d0];
    struct QInitRoutePointHistory *q;
};

void Axe_InitRoutePointHistory(struct S080FB1DC *s)
{
    struct QInitRoutePointHistory *q = s->q;
    s32 i;
    for (i = 0; i < 3; i++)
        q->v[i] |= 0xff;
    q->w = 0;
}
