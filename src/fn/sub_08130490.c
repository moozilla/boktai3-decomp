#include "global.h"
void sub_08130490(u8 *p)
{
    u8 *q = p + 0x2b1;
    u16 *h;
    u32 m;
    if (*q != 0)
        *q = 0;
    m = 2;
    h = (u16 *)(p + 0x2d4);
    if (*h & m)
        *(s32 *)(p + 0x2ac) += 1;
}
