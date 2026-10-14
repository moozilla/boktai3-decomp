#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_SetState(struct TetherActor *, u32);
u8 TetherActor_GetRequestMode(void)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    u8 r;
    if (p)
        r = p->requestMode;
    else
        r = 0;
    return r;
}
