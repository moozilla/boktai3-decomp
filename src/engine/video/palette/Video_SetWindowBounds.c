#include "global.h"
#include "gba/io_reg.h"
#include "engine/video/render.h"

void Video_SetWindowBounds(u32 window, u32 left, u32 top, u32 right, u32 bottom)
{
    if (window == 0) {
        gWindowShadow0 = (left << 8) | right;
        REG_WIN0H = gWindowShadow0;
        gWindowShadow2 = (top << 8) | bottom;
        REG_WIN0V = gWindowShadow2;
    } else {
        gWindowShadow3 = (left << 8) | right;
        REG_WIN1H = gWindowShadow3;
        gWindowShadow1 = (top << 8) | bottom;
        REG_WIN1V = gWindowShadow1;
    }
}
