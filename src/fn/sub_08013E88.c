#include "global.h"
void *sub_08013C14(void);
u8 *sub_08013C2C(void *, u32);
void sub_08013424(u8 *);
void sub_08013440(u8 *);
s32 sub_08013E88(u32 a, u32 m)
{
    void *p = sub_08013C14();
    u8 *q;
    if (p == 0 || (q = sub_08013C2C(p, a)) == 0)
        return -1;
    q[2] = m;
    if ((u8)m == 1)
        sub_08013424(q + 0xc);
    else if ((u8)m == 2)
        sub_08013440(q + 0xc);
    return 0;
}
