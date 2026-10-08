#include "global.h"
void sub_081453F0(u8 *p)
{
    u32 *q = (u32 *)(p + 0x8c);
    q[2] &= ~2;
}
