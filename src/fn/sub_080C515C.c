#include "global.h"
void sub_080C515C(u8 *p)
{
    u32 *q = (u32 *)(p + 0x2d0);
    u32 m = -3;
    *q = *q & m;
}
