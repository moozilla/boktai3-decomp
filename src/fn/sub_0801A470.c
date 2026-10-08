#include "global.h"
struct Z { u32 x; u32 y; };
void sub_0821983C(u8 *, u8 *, u32, u32, u32, u32, u32, struct Z *);
void sub_0801A470(u8 *p)
{
    struct Z z;
    u32 m = 0xFFFF0000;
    z.x = 0x800000;
    z.y &= m;
    sub_0821983C(p + 0x5b4, p + 0x6c, 0x189, 0x11, 0, 0, 0x3c, &z);
}
