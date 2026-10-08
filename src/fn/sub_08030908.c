#include "global.h"

struct S { u8 f[0x1ec]; u8 *a; };
extern u8 gUnk_030051A0[];
void sub_08030908(struct S *p)
{
    CpuSet(p->a + 0x1f4, gUnk_030051A0, 0x04000008);
}
