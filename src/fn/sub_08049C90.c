#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_ResetInteraction(void)
{
    if (gUnk_02000488) {
        TetherActor_SetState((struct TetherActor *)gUnk_02000488, 0);
        *(u16 *)(gUnk_02000488 + 0x41C) = 1;
    }
}
