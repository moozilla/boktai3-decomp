#include "projected_display.h"
/* Observed callers ignore the result; non-null fallthrough retains r0. */
u32 ProjectedDisplay_SetHeight(struct ProjectedDisplay *display, s32 height)
{
    if (!display)
        return 0;
    display->height = height;
}
