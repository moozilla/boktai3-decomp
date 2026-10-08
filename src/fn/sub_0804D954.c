#include "global.h"
struct P { u8 f[0x5c]; u16 f5c; u8 f5e[6]; u32 f64; };
void sub_0804D954(struct P *p)
{
    u16 *q = &p->f5c;
    u32 v = *q - 0x46;
    u32 z = 0;
    *q = v;
    if ((s32)(v << 16) <= 0) {
        u32 two;
        *q = z;
        two = 2;
        q -= 7;
        *q = two;
        p->f64 = z;
    } else {
        p->f64 += 1;
    }
}
