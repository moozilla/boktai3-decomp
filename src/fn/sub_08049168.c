#include "global.h"

struct S { u8 f[0x255]; u8 a; u16 b; };
void sub_08049168(struct S *p, u32 v)
{
    p->a = v;
    p->b = 0;
}
