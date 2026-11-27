#include "global.h"

u8 SunClock_GetMoonPhase(void)
{
    u8 *p = (u8 *)0x03005430;
    return p[0x16];
}
