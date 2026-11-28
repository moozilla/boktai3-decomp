#include "ui/hud/countdown_timer.h"

u16 CountdownTimer_GetRemaining(void)
{
    if (!gCountdownTimer)
        return 0;
    return gCountdownTimer->remaining;
}

void CountdownTimer_Stop(void)
{
    if (gCountdownTimer) {
        Hud_ClearTimer();
        gCountdownTimer->remaining = 0;
        Task_Kill(gCountdownTimer);
    }
}

void CountdownTimer_StopFromScript(void)
{
    if (gCountdownTimer)
        CountdownTimer_Stop();
}

s32 CountdownTimer_Update(struct CountdownTimer *timer)
{
    if (--timer->remaining == 0) {
        if (timer->completionScriptId)
            Script_Call(timer->completionScriptId, 0);
        CountdownTimer_Stop();
    }
    return 0;
}

void CountdownTimer_Destroy(void)
{
    gCountdownTimer = 0;
}

s32 CountdownTimer_InitFromScript(struct CountdownTimer *timer, void *unused1, void *unused2)
{
    if (Script_SeekToKeyword('t'))
        timer->remaining = Script_GetValue();
    else
        timer->remaining = 1800;
    if (Script_SeekToKeyword('p'))
        timer->completionScriptId = Script_GetValue();
    else
        timer->completionScriptId = 0;
    if (Script_GetKeywordValue('i', 0) == 0)
        Hud_RegisterTimer(&timer->remaining);
    gCountdownTimer = timer;
    return 0;
}

struct CountdownTimer *CountdownTimer_CreateFromScript(void *unused1, void *unused2)
{
    struct CountdownTimer *timer = gCountdownTimer, *created;
    if (!timer) {
        created = Task_Create(8, sizeof(struct CountdownTimer));
        if (created) {
            Task_SetCallbacks(created, (void (*)(void *))CountdownTimer_Update,
                              (void (*)(void *))CountdownTimer_Destroy);
            if (CountdownTimer_InitFromScript(created, unused1, unused2) < 0) {
                Task_Kill(created);
                return 0;
            }
        }
        timer = created;
    }
    return timer;
}

void CountdownTimer_AddFromScript(void)
{
    struct CountdownTimer *timer = gCountdownTimer;
    if (timer && !timer->locked && timer->remaining)
        timer->remaining += Script_GetKeywordValue('t', 0);
}
