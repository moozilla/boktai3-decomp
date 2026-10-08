#include "global.h"
extern u8 *gUnk_02000710;
s32 sub_081BD6D8(void)
{
    s16 v = *(s16 *)(gUnk_02000710 + 0x876);
    s32 r;
    if (v <= 1)
        r = 9;
    else if (v <= 9)
        r = 11 - v;
    else
        r = 2;
    return r;
}
