#include "stage_icon_display.h"
#define StageIconDisplay_NotifyCompletion sub_08055BE0
void StageIconDisplay_NotifyCompletion(struct StageIconDisplay *display)
{
    if (!display->scriptCompletion)
        sub_08055BA8(display);
    else
        sub_08055BC8(display);
}
