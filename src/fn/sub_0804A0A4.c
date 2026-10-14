#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_SetState(struct TetherActor *, u32);
u32 TetherActor_BeginKindFourStateTen(void)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    if (!p)
        return 0;
    if (p->kind != 4)
        return 0;
    TetherActor_SetState(p, 10);
    return 1;
}
