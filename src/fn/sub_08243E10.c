#include "global.h"
#include "runtime.h"
extern u32 gUnk_03006A34;
void Solar_InitTimer(void);
void Solar_Enable(void)
{
    gUnk_03006A34 = 0;
    gSolarEnabled = 1;
    Solar_InitTimer();
}
