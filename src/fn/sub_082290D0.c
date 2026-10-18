// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetMoonAgeTenths: Returns the cached lunar age stored in tenths of a day */
#define SunClock_GetMoonAgeTenths sub_082290D0

extern struct {
    u8 pad[20];
    u16 value;
} gUnk_03005430;

u16 sub_082290D0(void)
{
    return gUnk_03005430.value;
}
