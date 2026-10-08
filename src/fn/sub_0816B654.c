#include "global.h"
s8 sub_0816B3C0(u8 *);
void sub_0816B654(u8 *p, u8 a, u8 b)
{
    u32 z = 0;
    p[1] = a;
    p[2] = b;
    p[3] = sub_0816B3C0(p);
    p[0xb] = z;
}
