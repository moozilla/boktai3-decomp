#include "countdown_timer.h"
void CountdownTimer_StopFromScript(void)
{
    if (gCountdownTimer)
        CountdownTimer_Stop();
}
