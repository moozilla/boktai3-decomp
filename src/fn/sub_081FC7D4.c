#include "global.h"
struct S { u32 w0; u32 w4; u8 p[0x2a - 8]; u16 h2a; };
void sub_081FD4C0(void *);
void sub_081FC68C(void *);
u32 sub_081FC7D4(struct S *p)
{
    u32 r;
    if (p->w0 <= p->h2a) {
        sub_081FD4C0(p);
        { u32 m = 2; p->w4 = m | p->w4; }
        sub_081FC68C(p);
        r = 0;
    } else
        r = 1;
    return r;
}
