#include "tether_actor.h"
struct W {
    u32 a, b;
};
struct Q {
    u8 f[0x380];
    u16 v;
};
extern u8 *gUnk_02000488;
void TetherActor_SetState(u8 *, s32);
void TetherActor_CreateAuxiliarySprite(u8 *);
void TetherActor_UpdatePendingEffect(u8 *);
void TetherActor_BeginDisplayPositionState(void)
{
    u8 *g = gUnk_02000488;
    if (g != 0) {
        struct W *d;
        TetherActor_SetState(g, 8);
        TetherActor_CreateAuxiliarySprite(g);
        d = (struct W *)(g + 0x3b8);
        *d = **(struct W **)(g + 0x118);
        ((struct Q *)g)->v = 0x5dc;
        TetherActor_UpdatePendingEffect(g);
    }
}
