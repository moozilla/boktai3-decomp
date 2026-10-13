#include "global.h"
void *sub_08013C14(void);
u8 *sub_08013C2C(void *, u32);
void ActorShadow_Enable(u8 *);
void ActorShadow_Disable(u8 *);
s32 sub_08013E88(u32 a, u32 m)
{
    void *p = sub_08013C14();
    u8 *q;
    if (p == 0 || (q = sub_08013C2C(p, a)) == 0)
        return -1;
    q[2] = m;
    if ((u8)m == 1)
        ActorShadow_Enable(q + 0xc);
    else if ((u8)m == 2)
        ActorShadow_Disable(q + 0xc);
    return 0;
}
