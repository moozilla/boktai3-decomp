#include "global.h"

struct S { u8 f[1]; u8 a; u8 g[0x20a - 2 + 1]; u32 b; };
void sub_0821AD08(u32, u32);
void sub_08002C90(struct S *p)
{
    u32 z = 0;
    p->a = z;
    if (p->b != 0) {
        sub_0821AD08(p->b, 0);
        p->b = z;
    }
}
