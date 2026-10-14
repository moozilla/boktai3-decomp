#include "global.h"
struct W {
    u32 a, b;
};
struct Q {
    u8 f[0x380];
    u16 v;
};
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, s32);
void TetherActor_CreateAuxiliarySprite(u8 *);
void sub_080492A4(u8 *);
void sub_08049EBC(void)
{
    u8 *g = gUnk_02000488;
    if (g != 0) {
        struct W *d;
        sub_08049168(g, 8);
        TetherActor_CreateAuxiliarySprite(g);
        d = (struct W *)(g + 0x3b8);
        *d = **(struct W **)(g + 0x118);
        ((struct Q *)g)->v = 0x5dc;
        sub_080492A4(g);
    }
}
