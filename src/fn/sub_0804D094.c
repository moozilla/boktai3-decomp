#include "global.h"
extern u32 gUnk_030053F4;
void TetherActor_SetState(u8 *, s32);
void TetherActor_SetEntry(u8 *, s32);
void sub_0804B43C(u8 *);
void TetherActor_UpdateShake(u8 *);
void sub_0804CF5C(u8 *);
void sub_0804D094(u8 *p)
{
    u16 *q;
    if (p[0x255] != 0x10)
        TetherActor_SetState(p, 0x10);
    *(u16 *)(p + 0x136) &= ~4;
    q = (u16 *)(p + 0x38a);
    *q = 1;
    TetherActor_SetEntry(p, 0);
    if ((gUnk_030053F4 & 0x200) == 0) {
        u32 v = *(u16 *)(p + 0x3fc);
        if (v != 0) {
            sub_0804B43C(p);
        } else if (*(u16 *)(p + 0x384) != 0) {
            *q = v;
            TetherActor_UpdateShake(p);
        } else {
            sub_0804CF5C(p);
        }
    }
}
