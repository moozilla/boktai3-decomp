#include "tether_actor.h"
extern u8 *gUnk_02000488;
s32 TetherActor_GetParameter(void)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    s32 r;
    if (p)
        r = p->parameter;
    else
        r = -1;
    return r;
}
