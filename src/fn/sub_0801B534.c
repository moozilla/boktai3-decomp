#include "global.h"
void sub_0801B534(u8 *p, u32 v)
{
    *(u32 *)(p + 0x3c) = 0;
    p[0x38] = 1;
    *(u32 *)(p + 0x7a4) = v;
}
