#include "global.h"

void sub_0805F324(u8 *, u8 *);
void sub_0822B2F8(s32);
s32 sub_0805F758(u8 *, s32, u16);
void sub_08061C04(u8 *);

s32 sub_0806288C(u8 *p)
{
    u16 *q;
    sub_0805F324(p, p + 0x1D00);
    q = (u16 *)(p + 0x1AEE);
    if (*q == 0) sub_0822B2F8(0x10E);
    if (sub_0805F758(p + 0xCA8, 0x68, *q)) sub_08061C04(p);
    return 0;
}
