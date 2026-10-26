#include "countdown_timer.h"
u16 CountdownTimer_GetRemaining(void)
{
    if (!gCountdownTimer)
        return 0;
    return gCountdownTimer->remaining;
}
