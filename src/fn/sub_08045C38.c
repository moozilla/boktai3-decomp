#include "projected_display.h"
s32 ProjectedDisplay_Destroy(struct ProjectedDisplay *display)
{
    ProjectedDisplay_RemoveEntries(display);
    return 0;
}
