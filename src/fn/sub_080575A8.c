#include "countdown_timer.h"
s32 CountdownTimer_InitFromScript(struct CountdownTimer *timer, void *unused1, void *unused2)
{
    u32 value;
    if (Script_SeekToKeyword('t'))
        value = Script_GetValue();
    else
        value = 1800;
    timer->remaining = value;
    value = Script_SeekToKeyword('p');
    if (value)
        value = Script_GetValue();
    timer->completionScriptId = value;
    if (Script_GetKeywordValue('i', 0) == 0)
        Hud_RegisterTimer(&timer->remaining);
    gCountdownTimer = timer;
    return 0;
}
