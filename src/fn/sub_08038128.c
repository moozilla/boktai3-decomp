#include "global.h"

struct S { u8 f[0x60]; void *a; };
void sub_0803AC9C(void *);
void sub_08038128(struct S *p)
{
    if (p->a)
        sub_0803AC9C(p->a);
}
