#include "global.h"
struct S { u8 f[0x18]; u32 a; };
extern struct S *gUnk_020004B8;
u8 sub_08110B7C(u32);
void sub_0811117C(u32 x)
{
    if (sub_08110B7C(x))
        gUnk_020004B8->a = x;
}
