#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_BeginAuxiliaryState(struct TetherActor *);
void TetherActor_StartAuxiliaryState(void)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    if (p) {
        if (p->state != 5) {
            if (p->statusFlags == 0)
                TetherActor_BeginAuxiliaryState(p);
        }
    }
}
