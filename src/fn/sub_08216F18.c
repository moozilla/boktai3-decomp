#include "global.h"
extern u16 gUnk_0300154E, gUnk_03001550, gUnk_03001552, gUnk_03001554;
extern u16 gUnk_03004CF0, gUnk_03004CF8, gUnk_03004CFC, gUnk_03004D08;
void sub_08216F18(s32 window)
{
    if (window == 0) {
        gUnk_03004CF0 = gUnk_0300154E;
        gUnk_03004CFC = gUnk_03001552;
        *(vu16 *)0x4000040 = gUnk_03004CF0;
        *(vu16 *)0x4000044 = gUnk_03004CFC;
    } else {
        gUnk_03004D08 = gUnk_03001550;
        gUnk_03004CF8 = gUnk_03001554;
        *(vu16 *)0x4000042 = gUnk_03004D08;
        *(vu16 *)0x4000046 = gUnk_03004CF8;
    }
}