#include "global.h"

extern u32 gUnk_020001DC;
void sub_081151D0(u32);

s32 sub_081151F8(u32 a)
{
    sub_081151D0(a);
    gUnk_020001DC = a;
    return 0;
}
