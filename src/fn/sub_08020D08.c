#include "global.h"
extern s32 gUnk_0200047C;
s32 sub_080209AC(s32, s32);
s32 sub_08020D08(s32 a)
{
    s32 r;
    if (gUnk_0200047C)
        r = sub_080209AC(gUnk_0200047C, a);
    else
        r = -1;
    return r;
}
