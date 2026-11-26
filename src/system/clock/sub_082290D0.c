// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetMoonAgeTenths: Returns the cached lunar age stored in tenths of a day */
#define SunClock_GetMoonAgeTenths sub_082290D0

extern struct {
    u8 pad[20];
    u16 value;
} gSunClock;

u16 sub_082290D0(void)
{
    return gSunClock.value;
}
