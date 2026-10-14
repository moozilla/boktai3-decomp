#define ProjectedDisplay_Update               sub_08045C20
#define ProjectedDisplay_UpdateScreenPosition sub_08045B90
#define ProjectedDisplay_UpdateEntries        sub_08045ADC
#include "projected_display.h"
s32 ProjectedDisplay_Update(struct ProjectedDisplay *display)
{
    ProjectedDisplay_UpdateScreenPosition(display);
    ProjectedDisplay_UpdateEntries(display);
    return 0;
}
