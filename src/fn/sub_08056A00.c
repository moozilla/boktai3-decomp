#include "global.h"

struct S { u8 f[0x18]; u16 a; u8 g[2]; u16 b; u16 c; };
void sub_0821A284(u32, struct S *, u32);

s32 sub_08056A00(struct S *p)
{
    p->a = 0;
    p->b = 0;
    p->c = 0;
    sub_0821A284(0xFD22, p, 0);
    return 0;
}
