#include "global.h"

struct S { u8 f[0x18]; u8 a; u8 b; u8 c[6]; u32 d; };
void sub_08033C90(struct S *p, u8 v)
{
    p->a = v;
    p->b = 1;
    p->d = 0;
}
