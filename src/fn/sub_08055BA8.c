#include "stage_icon_display.h"
void StageIconDisplay_NotifyNative(struct StageIconDisplay *display)
{
    if (display->completion)
        display->completion(display->completionArgument, display);
}
