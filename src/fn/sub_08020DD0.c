#include "global.h"
extern s32 gUnk_0200047C;
s32 sub_080209D0(s32, s32);
s32 sub_08020DD0(s32 a)
{
    s32 r;
    if (gUnk_0200047C)
        r = sub_080209D0(gUnk_0200047C, a);
    else
        r = -1;
    return r;
}
