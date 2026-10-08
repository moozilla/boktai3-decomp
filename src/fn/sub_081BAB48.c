#include "global.h"
void sub_082195E0(u8 *);
void sub_081BAB48(u8 *p)
{
    u8 *q = p + 0x20;
    s32 i = 4;
    do {
        if (q[4]) sub_082195E0(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    {
        u32 *r = (u32 *)(p + 0x22c);
        if (*r) *r = 0;
    }
}
