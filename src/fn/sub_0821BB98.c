#include "global.h"
struct A { u8 p[4]; u16 *f4; u8 q[0x14]; s16 f1c; u16 f1e; u16 f20; u16 f22; };
extern struct A *gUnk_030052F4;
void sub_0821BB98(void)
{
    struct A *a = gUnk_030052F4;
    u16 *s = a->f4;
    a->f1c = -s[2];
    a->f1e = 1;
    a->f20 = s[2];
    a->f22 = 0xFFFF;
}
