#include "global.h"

extern u32 gUnk_020004C8;
void sub_08114C50(s32);

s32 sub_081143B8(void)
{
    s32 i;
    for (i = 0; i <= 0xf; i++)
        sub_08114C50(i);
    return gUnk_020004C8 = 0;
}
