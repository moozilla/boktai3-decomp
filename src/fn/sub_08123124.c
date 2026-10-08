#include "global.h"

extern u32 gUnk_02000144;
void sub_081230FC(u32);

s32 sub_08123124(u32 a)
{
    sub_081230FC(a);
    gUnk_02000144 = a;
    return 0;
}
