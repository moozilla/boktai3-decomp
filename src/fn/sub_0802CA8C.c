#include "global.h"

struct S { u8 f[0x18]; u32 a[1]; };
struct S *sub_0802C678(void);
u32 sub_0802CA8C(u32 i)
{
    struct S *p = sub_0802C678();
    if (!p) return 0;
    return p->a[i];
}
