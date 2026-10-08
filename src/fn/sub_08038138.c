#include "global.h"

struct S { u8 f[0x60]; void *a; };
void sub_0803ACC8(void *);
void sub_08038138(struct S *p)
{
    if (p->a)
        sub_0803ACC8(p->a);
}
