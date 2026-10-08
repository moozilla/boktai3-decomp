#include "global.h"

struct P { u8 f[0x84]; u16 a[2]; u16 b[2]; u8 g[0x94 - 0x8c - 0]; u32 c; u32 d[2]; };

void sub_08109BF8(struct P *p)
{
    u32 z = 0;
    u32 *d = p->d;
    u16 *a = p->a;
    s32 i = 1;
    do {
        *a = z;
        a[4] = z;
        *d++ = z;
        a++;
        i--;
    } while (i >= 0);
    p->c = 0;
}
