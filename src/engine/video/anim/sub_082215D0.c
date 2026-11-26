// COMPILER: old_agbcc
#include "global.h"
/* Math_AngleToFourWayDirection: Rounds an 8-bit angle to four sectors and rotates the direction index by one */

u32 Math_AngleToFourWayDirection(u16 angle)
{
    return ((((angle + 32) & 0xff) >> 6) + 1) & 3;
}
