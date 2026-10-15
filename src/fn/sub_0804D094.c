#include "tether_actor.h"
extern u32 gUnk_030053F4;
void TetherActor_SetState(struct TetherActor *, s32);
s32 TetherActor_SetEntry(struct TetherActor *, s32);
void TetherActor_UpdateLockedShake(struct TetherActor *);
void TetherActor_UpdateShake(struct TetherActor *);
void TetherActor_UpdateDirectedMovement(struct TetherActor *);
void TetherActor_UpdateDirectedState(struct TetherActor *p)
{
    u16 *q;
    if (p->state != 0x10)
        TetherActor_SetState(p, 0x10);
    *&p->chargeCollider.flags &= ~4;
    q = &p->glowMode;
    *q = 1;
    TetherActor_SetEntry(p, 0);
    if ((gUnk_030053F4 & 0x200) == 0) {
        u32 v = p->interactionLock;
        if (v != 0) {
            TetherActor_UpdateLockedShake(p);
        } else if (p->chargeReady != 0) {
            *q = v;
            TetherActor_UpdateShake(p);
        } else {
            TetherActor_UpdateDirectedMovement(p);
        }
    }
}
