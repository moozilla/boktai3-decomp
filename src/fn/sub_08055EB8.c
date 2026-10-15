#include "stage_icon_display.h"
void StageIconDisplay_SelectUpdate(struct StageIconDisplay *display, s32 variant)
{
    if (variant == 0)
        StageIconDisplay_SetUpdate(display, StageIconDisplay_TickReveal);
    else if (variant == 1)
        StageIconDisplay_SetUpdate(display, StageIconDisplay_TickDelay);
    else
        StageIconDisplay_SetUpdate(display, StageIconDisplay_TickReveal);
}
