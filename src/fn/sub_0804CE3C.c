#include "global.h"
void sub_08049168(u8 *, s32);
s32 sub_08049234(u8 *, s32);
void sub_0804CE3C(u8 *p)
{
    u16 *q;
    u32 z, v, m;
    if (p[0x255] != 0xe) sub_08049168(p, 0xe);
    q = (u16 *)(p + 0x136);
    v = *q;
    m = 4;
    z = 0;
    m |= v;
    *q = m;
    p[0x260] = z;
    if (sub_08049234(p, 9)) sub_08049168(p, 0xd);
}
