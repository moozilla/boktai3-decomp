#include "global.h"

struct S { u8 f[0x3c0]; u32 b; u8 g[0x9dc - 0x3c4]; u32 a; };
void sub_08047B78(u32);
void sub_08022AF4(struct S *p)
{
    sub_08047B78(p->a);
    p->b = 0;
}
