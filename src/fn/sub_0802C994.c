#include "global.h"

struct S { u8 f[0x58]; u32 a; u32 b; u32 c; };
void sub_0802C994(struct S *p)
{
    p->a = 0;
    p->b = 0;
    p->c = 0;
}
