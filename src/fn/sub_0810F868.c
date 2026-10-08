#include "global.h"
struct S { u8 f[0x18]; u32 a; u32 b; u32 c; };
u32 sub_0821A520(u32, u32);
void sub_082161B4(u32, u32, u32, u32, u32, u32, void *);
void CpuSet(const void *, void *, u32);
void sub_0810F868(struct S *s)
{
    u32 x;
    u32 r;
    s->a = sub_0821A520(0xC091, 0xCE15);
    s->b = sub_0821A520(0xC091, 0xA597);
    x = 3;
    sub_082161B4(2, 0, s->a, 0, 0, 1, &x);
    r = sub_0821A520(0x92B3, 0xA2F6) + 0x14;
    s->c = r;
    CpuSet((void *)r, (void *)0x03004FC0, 0x100);
}
