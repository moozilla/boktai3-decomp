#include "global.h"

extern u32 gUnk_020004D8;
void sub_0813A91C(s32);

s32 sub_0813A788(void)
{
    s32 i;
    for (i = 0; i <= 1; i++)
        sub_0813A91C(i);
    return gUnk_020004D8 = 0;
}
