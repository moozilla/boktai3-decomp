#include "global.h"
struct S { u8 f[0x58]; void *a58; };
void *sub_0821A520(u32, u32);
void sub_082161B4(u32, u32, void *, u32, u32, u32, void *);
void sub_081C13CC(struct S *p)
{
    u32 v[1];
    void *r;
    r = sub_0821A520(0xC091, 0x3536);
    p->a58 = r;
    v[0] = 0xb;
    sub_082161B4(1, 0, r, 0, 0, 1, v);
}
