#include "global.h"

struct S { u8 f[0x1d]; u8 a; };
void sub_0803ADA4(struct S *p)
{
    if (p->a) p->a = 0;
}
