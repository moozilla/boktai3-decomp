#include "global.h"
#include "runtime.h"
void Solar_ShutdownTimer(void);
void Solar_Disable(void)
{
    if (gSolarEnabled != 0) {
        gSolarEnabled = 0;
        Solar_ShutdownTimer();
    }
}
