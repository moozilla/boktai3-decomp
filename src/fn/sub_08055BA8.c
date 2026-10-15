#include "stage_icon_display.h"
#define StageIconDisplay_NotifyNative sub_08055BA8
void StageIconDisplay_NotifyNative(struct StageIconDisplay *display)
{
    if (display->completion)
        display->completion(display->completionArgument, display);
}
