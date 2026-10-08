#include "global.h"
struct S { u8 f[0x1c]; s32 a; };
extern struct S *gUnk_02000580;
u32 sub_0815A534(void)
{
    u32 r;
    if (!gUnk_02000580) goto z;
    if (gUnk_02000580->a == 4) goto o;
z:
    r = 0;
    goto e;
o:
    r = 1;
e:
    return r;
}
