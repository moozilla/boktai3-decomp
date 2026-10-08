#include "global.h"
struct S { u32 a; u32 b; u8 f[0x10]; u16 c; u16 d; };
void sub_081D7934(struct S *);
s32 sub_081D9D14(struct S *p)
{
    u32 two = 2;
    p->b |= two;
    if (p->a >= p->d) {
        p->c |= 0x40;
        sub_081D7934(p);
    }
    return 0;
}
