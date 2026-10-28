#include "global.h"
#include "runtime.h"
void Solar_InitTimer(void);
void Solar_Enable(void)
{
    gSolarGpioSaved = 0;
    gSolarEnabled = 1;
    Solar_InitTimer();
}
