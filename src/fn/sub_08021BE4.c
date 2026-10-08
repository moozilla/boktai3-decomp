#include "global.h"

struct S { u8 f[0x54]; u8 a; };
struct S *sub_0802141C(void);
u8 *sub_08021BE4(void)
{
    struct S *p = sub_0802141C();
    if (!p) return 0;
    return &p->a;
}
