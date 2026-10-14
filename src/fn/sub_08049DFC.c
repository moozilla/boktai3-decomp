#include "tether_actor.h"
extern u8 *gUnk_02000488;
u32 TetherActor_IsStateOne(void)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    if (!p)
        return 0;
    if (p->state == 1)
        return 1;
    return 0;
}
