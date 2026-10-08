#include "global.h"
struct S { u8 f[0x28]; s32 a; };
extern struct S *gUnk_020004B8;
s32 sub_08111140(void)
{
    if (!gUnk_020004B8) return -1;
    else return gUnk_020004B8->a;
}
