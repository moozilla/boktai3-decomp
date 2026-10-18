#include "global.h"

u32 SunClock_GetPackedDate(void)
{
    return *(u32 *)0x03005430;
}
