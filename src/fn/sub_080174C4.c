#include "global.h"
extern u8 *gUnk_02000710;
s32 sub_08228DB8(void);
s32 sub_080174C4(s16 *p)
{
    s32 r, v;
    if (*(s16 *)(gUnk_02000710 + 0x612) != 0)
        r = p[0x19];
    else
        r = (p[0x18] * p[0x19]) >> 6;
    v = sub_08228DB8();
    if ((u32)(v - 4) <= 1 || v == 0)
        r >>= 1;
    return r;
}
