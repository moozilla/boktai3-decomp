#include "countdown_timer.h"
void CountdownTimer_AddFromScript(void)
{
    struct CountdownTimer *timer = gUnk_0200049C;
    if (timer) {
        if (timer->locked == 0) {
            if (timer->remaining != 0)
                timer->remaining += Script_GetKeywordValue('t', 0);
        }
    }
}
