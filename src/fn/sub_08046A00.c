#include "global.h"

struct S { u8 f[0x9c]; u8 a; };
u32 sub_08214514(void *);
u32 sub_08046A00(struct S *p)
{
    sub_08214514(&p->a);
    return 0;
}
