#include "global.h"
extern u8 *gUnk_020000DC;
s32 sub_08032D8C(u8 *, s32);
s32 sub_08031DDC(s32 a)
{
    s32 r;
    if (gUnk_020000DC)
        r = sub_08032D8C(gUnk_020000DC + 0x18, a);
    else
        r = 0;
    return r;
}
