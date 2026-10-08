#include "global.h"
u16 *sub_0811D250(s32, u8 *, u32);
void sub_0811D2B8(u8 *a, u32 b)
{
    u16 *p = sub_0811D250(3, a, b);
    s32 i;
    for (i = 0; i <= 6; i++) {
        if (i != 3) *p = 0x10;
        p++;
    }
    *p = 1;
}
