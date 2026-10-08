#include "global.h"
struct S { u8 f[0x30]; void *a; u8 g[0x8]; void *b; u8 h[0x14]; s32 n; };
void sub_0821AD08(void *, u32);
void sub_0821A0C0(void *);
void sub_081C09F8(struct S *p)
{
    p->n = p->n + 1;
    if (p->n > 0x3f && p->b != 0) {
        sub_0821AD08(p->b, 0);
        sub_0821A0C0(p->a);
        sub_0821A0C0(p);
    }
}
