#include "global.h"

void sub_0805F324(u8 *, u8 *);
void sub_0822B2F8(s32);
s32 sub_0805F758(u8 *, s32, u16);
void sub_08061B48(u8 *);

s32 sub_08062778(u8 *p)
{
    u16 *q;
    sub_0805F324(p, p + 0x1D00);
    q = (u16 *)(p + 0x1AEE);
    if (*q == 0) sub_0822B2F8(0x276);
    if (sub_0805F758(p + 0xB88, 0x20, *q)) sub_08061B48(p);
    return 0;
}
