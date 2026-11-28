#include "global.h"
#include "gba/io_reg.h"
#include "engine/video/render.h"
void sub_08216F18(s32 window)
{
    if (window == 0) {
        gWindowShadow0 = gSavedWindowShadow0;
        gWindowShadow2 = gSavedWindowShadow2;
        REG_WIN0H = gWindowShadow0;
        REG_WIN0V = gWindowShadow2;
    } else {
        gWindowShadow3 = gSavedWindowShadow3;
        gWindowShadow1 = gSavedWindowShadow1;
        REG_WIN1H = gWindowShadow3;
        REG_WIN1V = gWindowShadow1;
    }
}
