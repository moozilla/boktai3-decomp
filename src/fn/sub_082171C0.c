#include "global.h"

extern s32 gUnk_03004DA4, gUnk_030051CC, gUnk_030051C0;
extern u16 gUnk_03004DC0[][16];
extern u16 *gUnk_030051E0;
extern u16 gUnk_03004FC0[];
extern u32 gUnk_03004DA0, gUnk_03004DA8, gUnk_030051D8;
extern u16 gUnk_030051F0[];
extern u16 gUnk_03005214, gUnk_030051C4, gUnk_030051DC, gUnk_03005210;
extern u32 gUnk_030051D4, gUnk_03004DAC, gUnk_030051C8;

void sub_082171C0(void)
{
    s32 i, j;
    u16 *p;

    gUnk_03004DA4 = 0;
    gUnk_030051CC = 0;
    gUnk_030051C0 = 0;
    p = gUnk_03004DC0[0];
    for (i = 0; i <= 15; i++) {
        for (j = 0; j < 16; j++)
            *p++ = 0;
        gUnk_030051F0[i] = 0;
    }
    gUnk_030051E0 = gUnk_03004FC0;
    gUnk_03004DA0 = 0x40;
    gUnk_03004DA8 = 0x40;
    gUnk_030051D8 = 0x40;
    gUnk_03005214 = 0x1084;
    gUnk_030051C4 = 0;
    gUnk_030051D4 = 0x40;
    gUnk_03004DAC = 0x40;
    gUnk_030051DC = 0x1084;
    gUnk_03005210 = 0;
    gUnk_030051C8 = 0;
}
