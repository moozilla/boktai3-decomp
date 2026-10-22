#include "global.h"
extern u16 gUnk_0300154E, gUnk_03001550, gUnk_03001552, gUnk_03001554;
extern u16 gUnk_03004CF0, gUnk_03004CF8, gUnk_03004CFC, gUnk_03004D08;
void sub_08216ED0(s32 window)
{
    if (window == 0) {
        gUnk_0300154E = gUnk_03004CF0;
        gUnk_03001552 = gUnk_03004CFC;
    } else {
        gUnk_03001550 = gUnk_03004D08;
        gUnk_03001554 = gUnk_03004CF8;
    }
}
