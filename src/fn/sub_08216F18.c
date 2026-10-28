#include "global.h"
#include "render.h"
void sub_08216F18(s32 window)
{
    if (window == 0) {
        gWindowShadow0 = gSavedWindowShadow0;
        gWindowShadow2 = gSavedWindowShadow2;
        *(vu16 *)0x4000040 = gWindowShadow0;
        *(vu16 *)0x4000044 = gWindowShadow2;
    } else {
        gWindowShadow3 = gSavedWindowShadow3;
        gWindowShadow1 = gSavedWindowShadow1;
        *(vu16 *)0x4000042 = gWindowShadow3;
        *(vu16 *)0x4000046 = gWindowShadow1;
    }
}