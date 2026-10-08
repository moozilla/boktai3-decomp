#include "global.h"
struct S { u8 f[0x724]; u16 a; };
void sub_0810FA2C(struct S *s)
{
    u16 v;
    s32 i;
    u16 *p;
    s->a = 0x3d;
    v = 0x3e;
    i = 0x11;
    p = (u16 *)((u8 *)s + 0x748);
    do {
        *p = v;
        p--;
    } while (--i >= 0);
}
