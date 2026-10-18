#include "global.h"
struct H {
    u8 f[6];
    u16 v;
    u8 g[4];
};
struct W {
    u32 a;
    u32 v;
};
static inline void orr(struct W *a, u32 m)
{
    a->v |= m;
}
s32 Nidhoggr_ChangeCompletedPartState(void *, s32, s32);
void Nidhoggr_StartSecondPartAction(u8 *p)
{
    if (p[0x724B] != 0)
        Nidhoggr_ChangeCompletedPartState(p, 1, 0x36);
    else
        Nidhoggr_ChangeCompletedPartState(p, 1, 0x15);
    ((struct H *)p)[2201].v |= 4;
    ((struct H *)p)[2019].v |= 4;
    orr(&((struct W *)p)[3566], 1);
}
