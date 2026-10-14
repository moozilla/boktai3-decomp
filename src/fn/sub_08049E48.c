#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_SetParameter(u32 a)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    if (p)
        p->parameter = a;
}
