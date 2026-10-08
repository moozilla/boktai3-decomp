#include "global.h"
s32 sub_080596D8(u8 *p, u32 a, u32 b)
{
    u8 *g = *(u8 **)(p + 0x6b0);
    u32 off = a << 2;
    u8 *t;
    u8 *e;
    t = g + 0xbe8;
    e = *(u8 **)(t + off);
    if (e[0xd3] == b) return 1;
    return 0;
}
