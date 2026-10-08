#include "global.h"
struct P { u8 pad[7]; u8 k; u8 pad2[0x14]; u32 v[5]; };
u32 sub_08032BEC(struct P *p)
{
    u32 r;
    switch (p->k) {
    case 0: r = p->v[0]; break;
    case 1: r = p->v[1]; break;
    case 2: r = p->v[2]; break;
    case 3: r = p->v[3]; break;
    case 4: r = p->v[4]; break;
    default: r = 0; break;
    }
    return r;
}
