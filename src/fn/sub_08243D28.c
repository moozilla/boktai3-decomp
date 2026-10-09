#include "global.h"
extern u32 gUnk_030035BC, gUnk_03006A4C, gUnk_03006A3C, gUnk_03006A38, gUnk_030035B8;
extern s32 gUnk_030035B0, gUnk_030035B4, gUnk_03006A48;
void sub_08243D28(void)
{
    u32 pins = *(vu16 *)0x080000C4;
    u32 shadow = gUnk_030035BC;
    u32 light;
    *(vu16 *)0x080000C4 = shadow;
    *(vu16 *)0x0400010C = -gUnk_03006A4C;
    light = 8;
    light &= pins;
    gUnk_03006A3C = light;
    if (gUnk_03006A38) {
        switch (gUnk_030035B0) {
        case 0:
            if (gUnk_030035B4 <= 9) shadow |= 2;
            else shadow &= -3;
            gUnk_030035BC = shadow;
            if (gUnk_030035B4 > 20 && light == 0) {
                gUnk_030035B0++;
                gUnk_030035B4 = light;
                gUnk_030035B8 = light;
            }
            break;
        case 1:
            if (light != 0) {
                gUnk_03006A48 = gUnk_030035B4 >> 1;
                gUnk_030035B0 = 2;
            }
            gUnk_030035B8 = light;
        case 2:
            gUnk_030035BC ^= 1;
            if (gUnk_030035B4 > 511) {
                gUnk_030035B0 = 0;
                gUnk_030035B4 = 0;
            }
            break;
        }
        gUnk_030035B4++;
    }
}
