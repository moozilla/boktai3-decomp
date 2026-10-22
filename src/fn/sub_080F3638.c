#include "global.h"

void sub_080F3638(u8 *p, u32 v)
{
    s32 i = 3;
    u32 *q = (u32 *)(p + 0x1b8);
    do {
        *q = v;
        q -= 7;
        i--;
    } while (i >= 0);
}
