// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetMoonAgeTenths: Returns the cached lunar age stored in tenths of a day */

extern struct {
    u8 pad[20];
    u16 value;
} gSunClock;

u16 SunClock_GetMoonAgeTenths(void)
{
    return gSunClock.value;
}
