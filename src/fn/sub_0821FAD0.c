#include "global.h"
struct A { u8 p[0x14]; void *f14; };
extern struct A *gUnk_030052F4;
void sub_0821F864(void *, u16 *, u32);
void sub_0821F998(void *, u16 *, u32);
void sub_0821FAD0(u16 *a, u32 b)
{
    void *t = gUnk_030052F4->f14;
    sub_0821F864(t, a, b);
    if (*a & 1) sub_0821F998(t, a, b);
}
