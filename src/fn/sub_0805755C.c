#include "countdown_timer.h"
void CountdownTimer_StopFromScript(void)
{
    if (gUnk_0200049C)
        CountdownTimer_Stop();
}
