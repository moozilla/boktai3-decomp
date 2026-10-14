#include "global.h"
void TetherActor_SetState(u8 *, s32);
s32 TetherActor_SetEntry(u8 *, s32);
void sub_0804CE3C(u8 *p)
{
    u16 *q;
    u32 z, v, m;
    if (p[0x255] != 0xe)
        TetherActor_SetState(p, 0xe);
    q = (u16 *)(p + 0x136);
    v = *q;
    m = 4;
    z = 0;
    m |= v;
    *q = m;
    p[0x260] = z;
    if (TetherActor_SetEntry(p, 9))
        TetherActor_SetState(p, 0xd);
}
