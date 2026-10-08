#include "global.h"
s32 sub_0811AE68(void *);
s32 sub_08113644(u8 *s, s32 y)
{
    u8 *p = s + 0xa0;
    s32 r = sub_0811AE68(p);
    s32 a = *(s32 *)(s + 0xc0);
    s32 b = *(s32 *)(p + 4);
    s32 c;
    b += a;
    c = *(s32 *)(p + 8);
    c += b;
    c >>= 12;
    c += 0x60;
    c += y;
    if (r <= c)
        return 1;
    return 0;
}
