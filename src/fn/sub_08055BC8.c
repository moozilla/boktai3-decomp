#include "stage_icon_display.h"
#define StageIconDisplay_CallCompletionScript sub_08055BC8
void StageIconDisplay_CallCompletionScript(struct StageIconDisplay *display)
{
    if (display->completionScriptId)
        Script_Call(display->completionScriptId, 0);
}
