#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_AddStatusFlags(u32 a, u32 b)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    if (p) {
        p->statusFlags |= a;
        *(u16 *)(gUnk_02000488 + 0x3E6) = b;
    }
}
