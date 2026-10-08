#include "global.h"
struct S { u8 f[0x20]; u32 a[2]; };
u32 sub_0821A520(u32, u16);
void CpuSet(const void *, void *, u32);
void sub_08111748(struct S *s, u32 a, u32 b)
{
    s32 i;
    for (i = 0; i <= 1; i++) {
        u32 r = 0;
        switch (i) {
        case 0:
            r = sub_0821A520(0x92B3, a);
            break;
        case 1:
            r = sub_0821A520(0x92B3, b);
            break;
        }
        s->a[i] = r + 0x14;
    }
    CpuSet((void *)s->a[0], (void *)0x03004FC0, 0x100);
}
