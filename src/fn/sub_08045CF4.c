#define ProjectedDisplay_GetHeight sub_08045CF4
#include "projected_display.h"
u32 ProjectedDisplay_GetHeight(struct ProjectedDisplay *display)
{
    if (!display)
        return 0;
    return display->height;
}
