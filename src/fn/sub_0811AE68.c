#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
u32 sub_0811AD8C(struct Z *);
u32 sub_0811AE68(u32 *p)
{
    struct Z z;
    z.a.lo = (p[0] << 4) >> 16;
    z.b.lo = (p[2] << 4) >> 16;
    return sub_0811AD8C(&z);
}
