#include "ui/hud/stage_icon_display.h"
s32 StageIconDisplay_Destroy(struct StageIconDisplay *display)
{
    struct ProjectedDisplayEntry *indicator;
    s32 i;
    Anim_Remove(&display->icon);
    for (i = 0; i < 3; i++) {
        indicator = &display->indicators[i];
        if (indicator->registered)
            Anim_Remove(indicator);
    }
    gStageIconDisplay = 0;
    return 0;
}
