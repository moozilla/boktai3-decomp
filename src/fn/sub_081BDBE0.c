#include "global.h"
void sub_081BDBE0(u8 *p, s32 b)
{
    s32 i = 0xf;
    u8 *q = p + 0x179f;
    u8 *r;
    do {
        *q = i;
        q--;
        i--;
    } while (i >= 0);
    i = 0x10;
    r = p + 0x1790;
    for (; i < 0x20; i++)
        r[i] = i + b;
}
