#include "ui/hud/stage_icon_display.h"
/* The ROM returns the word its void callee leaves in r0 (saved lr); callers only test its sign. */
s32 StageIconDisplay_SelectUpdate(struct StageIconDisplay *, s32);
s32 StageIconDisplay_InitFromScript(struct StageIconDisplay *display)
{
    s32 variant;
    s32 status;

    display->scriptCompletion = 1;
    if (Script_SeekToKeyword('m'))
        variant = Script_GetValue();
    else
        variant = 0;
    if (Script_SeekToKeyword('f'))
        display->option = Script_GetValue();
    else
        display->option = 0;
    if (Script_SeekToKeyword('p'))
        display->completionScriptId = Script_GetValue();
    else
        display->completionScriptId = 0;
    StageIconDisplay_InitAnimations(display, variant);
    status = StageIconDisplay_SelectUpdate(display, variant);
    gStageIconDisplay = display;
    return status;
}
