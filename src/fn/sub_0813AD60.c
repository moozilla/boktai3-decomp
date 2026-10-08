#include "global.h"

extern u32 gUnk_020001E0;
void sub_0813AD48(u32);

s32 sub_0813AD60(u32 a)
{
    sub_0813AD48(a);
    gUnk_020001E0 = a;
    return 0;
}
