#include "global.h"

extern void (*gUnk_08606434[])(void);
void sub_0824923C(u8 *, void *);

void sub_080F46D8(u8 *p)
{
    u32 m = 0x10;
    if ((p[0x8d] & m) == 0)
        sub_0824923C(p, gUnk_08606434[p[0x92]]);
}
