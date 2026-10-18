#include "global.h"
struct H {
    u8 f[6];
    u16 v;
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
void Nidhoggr_StartFirstPartAction(u8 *p)
{
    if (p[0x724B] != 0)
        Nidhoggr_ChangeCompletedPartState(p, 0, 0x38);
    else
        Nidhoggr_ChangeCompletedPartState(p, 0, 0x13);
    ((struct H *)p)[3291].v |= 4;
    ((struct H *)p)[3018].v |= 4;
    orr(&((struct W *)p)[3564], 1);
}
