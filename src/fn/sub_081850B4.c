#include "global.h"
struct P { u8 f0[0x196]; u16 w; u8 f1[0xB32 - 0x198]; u16 v; u8 f2[0xB3F - 0xB34]; u8 a; u8 b; u8 f3; u8 c; };
s32 sub_081850B4(struct P *p, u8 x)
{
    p->a = x;
    p->b = 0;
    p->c = 0;
    p->w |= 4;
    p->v &= 0xFFFD;
    return 1;
}
