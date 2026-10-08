#include "global.h"
void sub_0821AD08(u32, u32);
struct S { u8 f[0x40]; u32 a; u32 b; };
void sub_0811B784(struct S *p)
{
    if (p) {
        sub_0821AD08(p->a, 0);
        p->b = 0;
    }
}
