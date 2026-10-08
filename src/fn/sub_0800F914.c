#include "global.h"

struct S { u8 f[0x18]; void *a; };
void sub_08033778(void *);
u32 sub_0800F914(struct S *p)
{
    sub_08033778(p->a);
    return 0;
}
