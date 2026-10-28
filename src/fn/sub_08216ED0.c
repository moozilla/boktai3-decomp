#include "global.h"
#include "render.h"
void sub_08216ED0(s32 window)
{
    if (window == 0) {
        gSavedWindowShadow0 = gWindowShadow0;
        gSavedWindowShadow2 = gWindowShadow2;
    } else {
        gSavedWindowShadow3 = gWindowShadow3;
        gSavedWindowShadow1 = gWindowShadow1;
    }
}
