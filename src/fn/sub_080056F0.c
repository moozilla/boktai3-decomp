#include "global.h"
struct S { u8 f[0x18]; u16 n; u8 a; u8 b; u16 c; u8 d; u8 e; u32 q[4]; u32 r[2]; u32 s2[6]; };
extern struct S *gUnk_02000024;
extern u8 *gUnk_030042E4;
void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);

s32 sub_080056F0(struct S *p)
{
    void *m;
    u8 *b;
    gUnk_02000024 = p;
    if (p->n == 0)
        p->n = 8;
    m = sub_08219C40(p->n * 424);
    *(void **)((u8 *)p + 0x70) = m;
    if (m == 0) return -1;
    sub_08219DD8(m, p->n * 424);
    p->a = 0;
    p->d = 0;
    p->b = 0;
    p->c = 0;
    b = gUnk_030042E4;
    p->q[0] = (u32)(b + 0x80);
    p->q[1] = (u32)(b + 0xa0);
    p->q[2] = (u32)(b + 0xc0);
    p->q[3] = (u32)(b + 0xe0);
    CpuSet((void *)p->r[0], &p->r[0], 0x04000008);
    CpuSet((void *)p->r[1], (u8 *)p + 0x50, 0x04000008);
    return 0;
}
