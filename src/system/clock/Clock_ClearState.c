#include "global.h"
#include "system/clock/sun_clock.h"

void Memory_Clear(void *, u32);

void Clock_ClearState(void)
{
    Memory_Clear(&gSunClock, sizeof(gSunClock));
}
