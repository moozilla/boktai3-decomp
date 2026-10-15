#include "stage_icon_display.h"
s32 StageIconDisplay_InitFromScript(struct StageIconDisplay *display)
{
    s32 variant;
    u32 value;
    display->scriptCompletion = 1;
    if (Script_SeekToKeyword('m'))
        variant = Script_GetValue();
    else
        variant = 0;
    {
        u32 flag = Script_SeekToKeyword('f');
        if (flag)
            flag = Script_GetValue();
        display->option = flag;
    }
    value = Script_SeekToKeyword('p');
    if (value)
        display->completionScriptId = Script_GetValue();
    else
        display->completionScriptId = value;
    StageIconDisplay_InitAnimations(display, variant);
    {
        s32 status = StageIconDisplay_SelectUpdate(display, variant);
        gUnk_0200011C = display;
        return status;
    }
}
