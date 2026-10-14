// COMPILER: old_agbcc
#include "global.h"

void sub_0821E7D0(u16 *dst, u8 *src)
{
    u16 *s = *(u16 **)(src + 8);
    dst[0] = s[0];
    dst[2] = s[1];
}
