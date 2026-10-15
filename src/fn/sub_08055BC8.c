#include "stage_icon_display.h"
void StageIconDisplay_CallCompletionScript(struct StageIconDisplay *display)
{
    if (display->completionScriptId)
        Script_Call(display->completionScriptId, 0);
}
