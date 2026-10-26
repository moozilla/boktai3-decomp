#include "stage_icon_display.h"
void StageIconDisplay_RequestKill(void)
{
    if (gStageIconDisplay)
        gStageIconDisplay->killRequested = 1;
}
