#include "stage_icon_display.h"
void StageIconDisplay_SetUpdate(struct StageIconDisplay *display,
                                void (*update)(struct StageIconDisplay *))
{
    display->update = update;
    display->stateFrame = 0;
}
