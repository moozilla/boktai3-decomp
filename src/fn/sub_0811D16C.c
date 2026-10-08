#include "global.h"
struct S { u8 f[2]; u16 a; u8 g[8]; u16 b[1]; u16 c; u8 h[0x10]; u16 cnt; };
void CpuSet(const void *, void *, u32);
extern u16 gUnk_03004FE0[];
void sub_0811D16C(u16 *p)
{
    if (p[0x10] <= 1) {
        p[0x10]++;
    } else {
        u16 t;
        u16 *q;
        s32 i;
        u16 *g;
        p[0x10] = 0;
        t = p[7];
        i = 7;
        g = gUnk_03004FE0;
        q = p + 6;
        do {
            q[1] = q[0];
            q--;
            i--;
        } while (i > 1);
        p[1] = t;
        CpuSet(p, g, 0x04000008);
    }
}
