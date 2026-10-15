#include "countdown_timer.h"
#define CountdownTimer_AddFromScript sub_08057648
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
