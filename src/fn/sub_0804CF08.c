#include "global.h"
void TetherActor_SetState(u8 *, u32);
u32 TetherActor_SetEntry(u8 *, u32);
void sub_0804CF08(u8 *p)
{
    u16 *q;
    u32 z, v, m;
    if (p[0x255] != 0xF)
        TetherActor_SetState(p, 0xF);
    q = (u16 *)(p + 0x136);
    v = *q;
    m = 4;
    z = 0;
    m |= v;
    *q = m;
    p[0x260] = z;
    if (TetherActor_SetEntry(p, 0x12))
        TetherActor_SetState(p, *(u16 *)(p + 0x470));
}
