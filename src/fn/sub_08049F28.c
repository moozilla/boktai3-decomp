#include "tether_actor.h"
extern u8 *gUnk_02000488;
u32 TetherActor_TestStatusFlags(u32 m)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    if (!p)
        return 0;
    if (p->statusFlags & m)
        return 1;
    return 0;
}
