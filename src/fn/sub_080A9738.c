#include "global.h"
void sub_080A9738(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3d0);
    *(u16 *)(q + 0x8d8) = 0;
    q[0x8d4] = 1;
}
