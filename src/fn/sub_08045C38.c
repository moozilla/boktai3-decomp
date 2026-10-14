#define ProjectedDisplay_Destroy       sub_08045C38
#define ProjectedDisplay_RemoveEntries sub_08045B74
#include "projected_display.h"
s32 ProjectedDisplay_Destroy(struct ProjectedDisplay *display)
{
    ProjectedDisplay_RemoveEntries(display);
    return 0;
}
