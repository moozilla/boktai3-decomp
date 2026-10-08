#include "global.h"

void sub_08032C54(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    p[2] = a;
    p[3] = b;
    p[4] = c;
    p[5] = d;
    p[0] = a;
    p[1] = b;
}
