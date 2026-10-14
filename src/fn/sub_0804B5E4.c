#include "global.h"
extern u32 gUnk_030053F4;
void TetherActor_SetState(u8 *, u32);
void TetherActor_SetEntry(u8 *);
void sub_0804B43C(u8 *);
void TetherActor_UpdateShake(u8 *);
void sub_0804B58C(u8 *);
void sub_0804B5E4(u8 *p)
{
    if (p[0x255] != 0)
        TetherActor_SetState(p, 0);
    {
        u16 *q = (u16 *)(p + 0x136);
        u32 v = *q;
        u32 m = 0xFFFB;
        u32 z;
        m &= v;
        z = 0;
        *q = m;
        *(u16 *)(p + 0x38A) = z;
    }
    TetherActor_SetEntry(p);
    if (!(gUnk_030053F4 & 0x200)) {
        if (p[0x1A] != 9) {
            u16 *r3 = (u16 *)(p + 0x3FC);
            if (*r3 != 0) {
                sub_0804B43C(p);
            } else {
                u32 v = *(u16 *)(p + 0x384);
                if (v != 0) {
                    TetherActor_UpdateShake(p);
                } else if (p[0x472] != 0) {
                    u32 t = *(u16 *)(p + 0x3FA);
                    u32 t2;
                    *(u16 *)(p + 0x3F8) = t;
                    t2 = *(u16 *)(p + 0x40E);
                    *(u16 *)(p + 0x40C) = t2;
                    *r3 = v;
                } else {
                    sub_0804B58C(p);
                }
            }
        }
    }
}
