#include "tether_actor.h"
extern u8 *gUnk_02000488;
struct ContactPosition *TetherActor_GetPosition(void)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    if (!p)
        return 0;
    return &p->position;
}
