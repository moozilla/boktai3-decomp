#include "stage_icon_display.h"
void StageIconDisplay_NotifyCompletion(struct StageIconDisplay *display)
{
    if (!display->scriptCompletion)
        StageIconDisplay_NotifyNative(display);
    else
        StageIconDisplay_CallCompletionScript(display);
}
