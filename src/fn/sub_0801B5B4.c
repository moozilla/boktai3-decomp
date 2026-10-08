#include "global.h"
void sub_0801B5B4(u8 *p, u8 *q, u32 i)
{
    u32 z = 0;
    p[0xc] = i;
    q += 0x15;
    p[0xd] = q[i];
    p[0xe] = z;
    p[0xf] = z;
}
