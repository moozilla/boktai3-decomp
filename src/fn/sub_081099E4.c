#include "global.h"

extern u32 gUnk_0300546C;
void Solar_ResumeMeasurement(void);
void LinkConnectionMenu_HideTextWindow(s32);

s32 LinkConnectionMenu_OnDestroy(s32 a)
{
    Solar_ResumeMeasurement();
    gUnk_0300546C = 0;
    LinkConnectionMenu_HideTextWindow(a);
    return 1;
}
