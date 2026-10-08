#include "global.h"

extern u32 gUnk_020004BC;
void sub_08112C08(s32);

s32 sub_08112A70(void)
{
    s32 i;
    for (i = 0; i <= 7; i++)
        sub_08112C08(i);
    return gUnk_020004BC = 0;
}
