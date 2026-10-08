#include "global.h"
struct Z { u32 x, y; };
struct C { u32 w[8]; };
struct C *sub_0821A520(u32, u32);
void sub_082196C4(u8 *, struct C *);
void sub_0821983C(u8 *, u8 *, u32, u32, u32, u32, u32, struct Z *);
void sub_0805E3FC(u8 *p)
{
    struct Z z;
    struct C *r = sub_0821A520(0xCB05, 0x5D04);
    u8 *a;
    s32 j;
    u32 zero;
    *(struct C *)p = *r;
    sub_082196C4(p, r);
    {
        u32 m = 0xFFFF0000;
        z.x = 0;
        z.y &= m;
    }
    zero = 0;
    a = p + 0x20;
    j = 1;
    do {
        sub_0821983C(a, p, 0, 0x31, zero, zero, 0x3c, &z);
        a += 0x60;
        j--;
    } while (j >= 0);
}
