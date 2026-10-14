#include "projected_display.h"
s32 ProjectedDisplay_Update(struct ProjectedDisplay *display)
{
    ProjectedDisplay_UpdateScreenPosition(display);
    ProjectedDisplay_UpdateEntries(display);
    return 0;
}
