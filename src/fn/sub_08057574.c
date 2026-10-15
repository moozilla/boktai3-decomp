#include "countdown_timer.h"
s32 CountdownTimer_Update(struct CountdownTimer *timer)
{
    u32 value = timer->remaining - 1;
    timer->remaining = value;
    if ((s32)(value << 16) == 0) {
        if (timer->completionScriptId)
            Script_Call(timer->completionScriptId, 0);
        CountdownTimer_Stop();
    }
    return 0;
}
