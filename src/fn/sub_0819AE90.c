#include "global.h"

void sub_0819AE90(u8 *p)
{
    u32 z;
    u8 v = p[1] + 1;
    z = 0;
    p[1] = v;
    p[2] = z;
    *(u32 *)(p + 0xc) = z;
    *(u32 *)(p + 8) = z;
}
