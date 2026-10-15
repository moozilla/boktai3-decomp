#include "stage_icon_display.h"
#define StageIconDisplay_SetUpdate sub_08055B90
void StageIconDisplay_SetUpdate(struct StageIconDisplay *display,
                                void (*update)(struct StageIconDisplay *))
{
    display->update = update;
    display->stateFrame = 0;
}
