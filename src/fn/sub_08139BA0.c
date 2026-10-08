#include "global.h"

extern u32 gUnk_020004D0;
void sub_08139F58(s32);

s32 sub_08139BA0(void)
{
    s32 i;
    for (i = 0; i <= 5; i++)
        sub_08139F58(i);
    return gUnk_020004D0 = 0;
}
