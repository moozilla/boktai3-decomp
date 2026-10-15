#include "global.h"
#define AreaEffect_CreateOnce sub_0804EF78
extern s32 gUnk_0200010C;
extern u32 gUnk_030053F4;
s32 sub_0804EB90(u32, u32);
s32 sub_0804EF30(u32, u32);
s32 AreaEffect_CreateOnce(u32 a, u32 b)
{
    s32 r = gUnk_0200010C;
    if (r == 0) {
        if ((gUnk_030053F4 & 0x800) == 0)
            r = sub_0804EB90(a, b);
        else
            r = sub_0804EF30(a, b);
    }
    return r;
}
