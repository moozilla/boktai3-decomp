#include "countdown_timer.h"
#define CountdownTimer_Stop sub_08057538
void CountdownTimer_Stop(void)
{
    if (gUnk_0200049C) {
        struct CountdownTimer *timer;
        sub_08178404();
        timer = gUnk_0200049C;
        timer->remaining = 0;
        Task_Kill(timer);
    }
}
