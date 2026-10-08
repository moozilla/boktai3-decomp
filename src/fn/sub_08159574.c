#include "global.h"

void sub_08159574(u8 *p, s32 i, s32 d)
{
    if (p != 0) {
        u8 *e;
        p += 0x450;
        e = p + i;
        if (*e <= d)
            *e = 0;
        else
            *e -= d;
    }
}
