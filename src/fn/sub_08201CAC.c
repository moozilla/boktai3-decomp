#include "global.h"
struct V { u16 a; u16 b; u32 c; };
struct S { u8 p0[0x18]; s32 w18; u8 p1[0x40 - 0x1c]; struct V v; u8 p2[0x1a8 - 0x48]; s16 h1a8; };
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_081263E0(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void sub_08201388(void *);
u32 sub_08201CAC(struct S *p)
{
    if (p->h1a8 == 0) {
        if (p->w18 <= 0x23) {
            struct V v;
            u16 *q = (u16 *)&v;
            u32 idx, t;
            v = p->v;
            q[1] += 0xc8;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            idx = gUnk_03005308;
            t = *(u8 *)&gUnk_0203B400[idx];
            sub_081263E0(&v, 0x80, 0x20, 0xc, 0xc, 0x18, t, 0x20, 0x20, 1, 2, 0x200);
        }
        if (p->w18 == 1)
            sub_08201388(p);
    }
    return 0;
}
