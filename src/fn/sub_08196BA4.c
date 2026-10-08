#include "global.h"
struct P { u8 f[0x5404]; u16 a, b, c; u8 g[0x724A-0x540A]; u8 s; };
void sub_08196BA4(struct P *p)
{
    u8 *q = &p->s;
    u32 z = *q;
    if (z == 0) {
        p->a = z;
        p->b = z;
        p->c = 0xF980;
        (*q)++;
    }
}
