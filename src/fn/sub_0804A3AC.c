#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_RequestPositionSample(void)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    if (p)
        p->destroyRequested = 1;
}
