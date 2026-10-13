#include "global.h"

u32 sub_0803EC88(u32 a, u8 *b, u8 *e)
{
    u32 m;
    u32 *p;
    if (e[0x64] != 0)
        e[0x64] = 0;
    *(u32 *)(b + 0x858) |= 0x20;
    m = 0x10;
    p = (u32 *)(b + 0x5fc);
    *p |= m;
    (*(u32 *)(e + 0x44))++;
    return 0;
}
