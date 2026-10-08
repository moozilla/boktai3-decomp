#include "global.h"
struct S { u8 f[0x1AEE]; u16 a; u8 g[0x1CF8-0x1AF0]; u32 b; };
void sub_0805F784(struct S *p, u32 v)
{
    p->b = v;
    p->a = 0;
}
