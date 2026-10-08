#include "global.h"

extern u32 gUnk_02000208;
void sub_0811E2F0(u32);

s32 sub_0811E318(u32 a)
{
    sub_0811E2F0(a);
    gUnk_02000208 = a;
    return 1;
}
