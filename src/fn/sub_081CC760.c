#include "global.h"
struct S { u8 f[4]; u8 b4; u8 b5; u8 f6; u8 b7; u8 b8; u8 g[0x14 - 9]; u32 c14; u8 g2[0x168 - 0x18]; u8 d[1]; };
void sub_08020D68(void *, s32);
void sub_081CC760(u32 a, struct S *p)
{
    if (p->b8 != 0) {
        p->b8 = 0;
        p->b7 = 0;
        p->b5 = 0x16;
    }
    if (p->b7 != 0)
        sub_08020D68(&p->d, 1);
    p->c14++;
}
