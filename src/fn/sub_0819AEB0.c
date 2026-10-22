#include "global.h"

void sub_0819AEB0(u8 *p)
{
    u32 z;
    u8 v = p[2] + 1;
    z = 0;
    p[2] = v;
    *(u32 *)(p + 0xc) = z;
}
