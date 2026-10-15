#include "stage_icon_display.h"
#define StageIconDisplay_SelectUpdate sub_08055EB8
void StageIconDisplay_SelectUpdate(struct StageIconDisplay *display, s32 variant)
{
    if (variant == 0)
        sub_08055B90(display, sub_08055C04);
    else if (variant == 1)
        sub_08055B90(display, sub_08055D38);
    else
        sub_08055B90(display, sub_08055C04);
}
