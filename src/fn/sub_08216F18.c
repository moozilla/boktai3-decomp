#include "global.h"
#include "render.h"
extern u16 gUnk_0300154E, gUnk_03001550, gUnk_03001552, gUnk_03001554;
void sub_08216F18(s32 window)
{
    if (window == 0) {
        gWindowShadow0 = gUnk_0300154E;
        gWindowShadow2 = gUnk_03001552;
        *(vu16 *)0x4000040 = gWindowShadow0;
        *(vu16 *)0x4000044 = gWindowShadow2;
    } else {
        gWindowShadow3 = gUnk_03001550;
        gWindowShadow1 = gUnk_03001554;
        *(vu16 *)0x4000042 = gWindowShadow3;
        *(vu16 *)0x4000046 = gWindowShadow1;
    }
}