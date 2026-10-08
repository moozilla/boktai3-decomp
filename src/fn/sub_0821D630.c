#include "global.h"
struct R { u16 f0; u16 f2; u8 f4; u8 f5; u16 f6; };
void sub_0821D630(struct R *r, u16 a, u32 b, u32 c, u8 d, u16 e)
{
    r->f2 = a;
    r->f0 = 0;
    r->f4 = (b << 4) | c;
    r->f5 = d;
    r->f6 = e;
}
