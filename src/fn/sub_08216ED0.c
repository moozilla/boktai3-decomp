#include "global.h"
#include "render.h"
extern u16 gUnk_0300154E, gUnk_03001550, gUnk_03001552, gUnk_03001554;
void sub_08216ED0(s32 window)
{
    if (window == 0) {
        gUnk_0300154E = gWindowShadow0;
        gUnk_03001552 = gWindowShadow2;
    } else {
        gUnk_03001550 = gWindowShadow3;
        gUnk_03001554 = gWindowShadow1;
    }
}
