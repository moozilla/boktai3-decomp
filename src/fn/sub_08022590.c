#include "global.h"

struct S { u8 f[0x9d8]; u32 a; };
struct S *sub_0802140C(u32);
void sub_08043F5C(void);
void sub_08022590(void)
{
    struct S *p = sub_0802140C(0);
    if (p && p->a != 0)
        sub_08043F5C();
}
