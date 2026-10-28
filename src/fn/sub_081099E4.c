#include "global.h"
#include "ram.h"

void Solar_ResumeMeasurement(void);
void LinkConnectionMenu_HideTextWindow(s32);

s32 LinkConnectionMenu_OnDestroy(s32 a)
{
    Solar_ResumeMeasurement();
    gClockSynced = 0;
    LinkConnectionMenu_HideTextWindow(a);
    return 1;
}
