#include "global.h"

void sub_0819AE84(u8 *p, u8 substate)
{
    u32 z = 0;
    p[1] = substate;
    p[2] = z;
    *(u32 *)(p + 0xc) = z;
    *(u32 *)(p + 8) = z;
}
