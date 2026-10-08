#include "global.h"

struct S { u8 f[0x18]; u8 a; };
void sub_08048384(struct S *p, u32 v)
{
    if (p)
        p->a = v;
}
