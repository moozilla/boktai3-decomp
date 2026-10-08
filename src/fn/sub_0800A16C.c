#include "global.h"

struct S { u8 f[2]; u8 c; u8 g[4]; u8 a; u8 h[2]; u16 n; };
void sub_0800A16C(u32 x, struct S *p)
{
    u16 n;
    if (p->a != 0)
        p->a = 0;
    n = p->n++;
    if (n > 0x1e) {
        u32 t = 2;
        p->c = t;
        p->n = 0;
        p->a = 1;
    }
}
