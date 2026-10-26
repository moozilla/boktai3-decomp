#include "stage_icon_display.h"
s32 StageIconDisplay_Destroy(struct StageIconDisplay *display)
{
    struct ProjectedDisplayEntry *indicator;
    s32 i;
    Anim_Remove(&display->icon);
    indicator = display->indicators;
    i = 2;
    do {
        if (indicator->registered)
            Anim_Remove(indicator);
        indicator++;
    } while (--i >= 0);
    {
        u32 zero = 0;
        gStageIconDisplay = (void *)zero;
    }
    return 0;
}
