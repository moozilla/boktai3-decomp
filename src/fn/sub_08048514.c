#include "global.h"

struct S { u8 f[0x44]; u8 a; };
extern u32 gUnk_020000F4;
void sub_082195E0(void *);
u32 sub_08048514(struct S *p)
{
    sub_082195E0(&p->a);
    return gUnk_020000F4 = 0;
}
