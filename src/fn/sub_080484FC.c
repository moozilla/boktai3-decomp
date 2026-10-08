#include "global.h"

struct S { u8 f[0x18]; u32 a; };
void sub_0804846C(void);
u32 sub_080484FC(struct S *p)
{
    sub_0804846C();
    p->a++;
    return 0;
}
