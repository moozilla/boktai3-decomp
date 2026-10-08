#include "global.h"
struct S { u8 f[0x5c]; u8 *a; };
u8 *sub_0821A520(u32, u32);
void sub_081C1408(struct S *p)
{
    p->a = sub_0821A520(0x92B3, 0x204) + 0x14;
    CpuSet(p->a, (void *)0x03004FC0, 0x04000028);
}
