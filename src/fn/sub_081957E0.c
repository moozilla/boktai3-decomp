#include "global.h"

void Nidhoggr_ShufflePartOrder(u8 *);

u8 Nidhoggr_GetNextShuffledPart(u8 *p)
{
    u8 *q = p + 0x725E;
    u8 *t;
    u8 v;
    u8 r;
    if (*q > 8) {
        Nidhoggr_ShufflePartOrder(p);
    }
    v = *q;
    t = p + 0x725F;
    r = t[v];
    *q = v + 1;
    return r;
}
