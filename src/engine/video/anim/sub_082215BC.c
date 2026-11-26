// COMPILER: old_agbcc
#include "global.h"
/* Math_AngleToEightWayDirection: Rounds an 8-bit angle to eight sectors and rotates the direction index by three */
#define Math_AngleToEightWayDirection sub_082215BC

u32 sub_082215BC(u16 angle)
{
    return ((((angle + 16) & 0xff) >> 5) + 3) & 7;
}
