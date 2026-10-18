// COMPILER: old_agbcc
#include "global.h"
/* Route_GetCurrentPointXZ: Copies XZ coordinates from the route's current point pointer */
#define Route_GetCurrentPointXZ sub_0821E7D0

void sub_0821E7D0(u16 *dst, u8 *src)
{
    u16 *s = *(u16 **)(src + 8);
    dst[0] = s[0];
    dst[2] = s[1];
}
