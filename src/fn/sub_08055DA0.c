#include "stage_icon_display.h"
#define StageIconDisplay_Destroy sub_08055DA0
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
        gUnk_0200011C = (void *)zero;
    }
    return 0;
}
