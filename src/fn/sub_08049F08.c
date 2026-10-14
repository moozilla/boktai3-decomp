#include "tether_actor.h"
extern u8 *gUnk_02000488;
u32 TetherActor_GetStatusFlags(void)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    u32 r;
    if (p)
        r = p->statusFlags;
    else
        r = 0;
    return r;
}
