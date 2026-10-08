#include "global.h"

struct S { u8 f[0xbc]; u32 a[1]; };
u32 sub_08227E90(void);
void sub_08041E04(struct S *p, u32 i)
{
    p->a[i] = sub_08227E90();
}
