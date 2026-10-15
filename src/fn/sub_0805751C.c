#include "countdown_timer.h"
#define CountdownTimer_GetRemaining sub_0805751C
u16 CountdownTimer_GetRemaining(void)
{
    if (!gUnk_0200049C)
        return 0;
    return gUnk_0200049C->remaining;
}
