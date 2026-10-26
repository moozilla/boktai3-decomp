#include "countdown_timer.h"
void CountdownTimer_Stop(void)
{
    if (gCountdownTimer) {
        struct CountdownTimer *timer;
        Hud_ClearTimer();
        timer = gCountdownTimer;
        timer->remaining = 0;
        Task_Kill(timer);
    }
}
