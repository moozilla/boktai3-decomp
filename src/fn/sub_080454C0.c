#include "global.h"

struct S { u8 f[0x1a]; u16 a; };
void sub_080454C0(u32 a, u32 b, struct S *p)
{
    *(u32 *)p = p->a;
}
