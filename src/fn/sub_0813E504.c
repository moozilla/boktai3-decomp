#include "global.h"

extern s32 gUnk_03005308;
extern u8 gUnk_0203B400[];
s32 Mod(s32, s32);

s32 sub_0813E504(void)
{
    s32 r;

    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    r = Mod(*(u16 *)(gUnk_0203B400 + (gUnk_03005308 << 1)), 100);
    if (r > 20) {
        r = 0;
    } else {
        r = 0x80000;
    }
    return r;
}
