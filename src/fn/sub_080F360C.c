#include "global.h"

struct P { u8 f[0xb0c]; u16 d[16]; u16 s[16]; u8 g[0xba0 - 0xb4c]; u16 *src; };

void sub_080F360C(struct P *p)
{
    u16 **sp = &p->src;
    u32 z = 0;
    u16 *s = *sp;
    u16 *d = p->d;
    s32 i = 15;
    do {
        *d = z;
        d[16] = *s;
        s++;
        d++;
        i--;
    } while (i >= 0);
}
