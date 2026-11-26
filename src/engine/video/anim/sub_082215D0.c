// COMPILER: old_agbcc
#include "global.h"
/* Math_AngleToFourWayDirection: Rounds an 8-bit angle to four sectors and rotates the direction index by one */
#define Math_AngleToFourWayDirection sub_082215D0

u32 sub_082215D0(u16 angle)
{
    return ((((angle + 32) & 0xff) >> 6) + 1) & 3;
}
