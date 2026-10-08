#include "global.h"

void sub_0813AFEC(u8 *, s32, s32);

void sub_08158ECC(u8 *p, s32 *a, s32 b)
{
    s32 *d;
    s32 x;
    s32 y;

    sub_0813AFEC(p, 8, 0);
    *(u16 *)(p + 0x4E8) = 0;
    d = (s32 *)(p + 0x498);
    x = a[0];
    y = a[1];
    d[0] = x;
    d[1] = y;
    *(u16 *)(p + 0x4A0) = b;
}
