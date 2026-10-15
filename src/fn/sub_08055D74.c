#include "stage_icon_display.h"
s32 StageIconDisplay_Tick(struct StageIconDisplay *display)
{
    if (display->killRequested)
        Task_Kill(display);
    else
        display->update(display);
    return 0;
}
