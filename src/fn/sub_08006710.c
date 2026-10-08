#include "global.h"

struct S { u8 f[0x104]; u8 a; u8 g[0x5f]; u8 b; };
void sub_082195E0(void *);
extern u32 gUnk_0200002C;
u32 sub_08006710(struct S *p)
{
    sub_082195E0(&p->a);
    sub_082195E0(&p->b);
    return gUnk_0200002C = 0;
}
