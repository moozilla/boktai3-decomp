#include "global.h"
struct S { u8 f[0x8E4]; u16 a[24]; };
void sub_08111D64(struct S *s)
{
    s32 i;
    u16 *p;
    u16 v;
    s->a[0] = 0x34;
    s->a[1] = 0x39;
    s->a[2] = 0x3a;
    s->a[3] = 0x3b;
    s->a[6] = 0x48;
    s->a[4] = 0x21;
    s->a[5] = 0x22;
    s->a[7] = 0x3c;
    v = 0x3e;
    p = &s->a[8];
    i = 0xe;
    do {
        *p = v;
        p++;
    } while (--i >= 0);
}
