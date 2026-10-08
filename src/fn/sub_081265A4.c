#include "global.h"
extern u32 gUnk_02000154;
void sub_08214514(u8 *);
u32 sub_081265A4(u8 *p)
{
    u8 *r = p + 0x88;
    u8 *q = p + 0x1c;
    s32 n = 0x2f;
    u32 z;
    do {
        if (*(s8 *)r >= 0)
            sub_08214514(q);
        r += 0x70;
        q += 0x70;
        n--;
    } while (n >= 0);
    z = 0;
    gUnk_02000154 = z;
    return z;
}
