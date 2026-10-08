#include "global.h"

struct S { u8 f[0x60]; void *a; };
void sub_0803ACD0(void *);
void sub_08038148(struct S *p)
{
    if (p->a)
        sub_0803ACD0(p->a);
}
