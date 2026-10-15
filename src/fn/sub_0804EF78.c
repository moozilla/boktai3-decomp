#include "global.h"
extern s32 gUnk_0200010C;
extern u32 gUnk_030053F4;
s32 AreaEffect_Create(u32, u32);
s32 AreaEffect_CreateLink(u32, u32);
s32 AreaEffect_CreateOnce(u32 a, u32 b)
{
    s32 r = gUnk_0200010C;
    if (r == 0) {
        if ((gUnk_030053F4 & 0x800) == 0)
            r = AreaEffect_Create(a, b);
        else
            r = AreaEffect_CreateLink(a, b);
    }
    return r;
}
