#include "global.h"

extern u32 gUnk_02000200;
void sub_0811B6C8(u32);

s32 sub_0811B714(u32 a)
{
    sub_0811B6C8(a);
    gUnk_02000200 = a;
    return 0;
}
