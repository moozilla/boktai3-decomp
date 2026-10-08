#include "global.h"

struct S { u8 f[0x1e]; u16 a; };
struct S *sub_08037DC0(void);
u32 sub_080397AC(void)
{
    struct S *p = sub_08037DC0();
    if (!p) return 0;
    return p->a;
}
