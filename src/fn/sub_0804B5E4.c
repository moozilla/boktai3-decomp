#include "tether_actor.h"
extern u32 gUnk_030053F4;
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_UpdateLockedShake(struct TetherActor *);
void TetherActor_UpdateShake(struct TetherActor *);
void TetherActor_UpdateInteractionValue(struct TetherActor *);
void TetherActor_UpdateIdleState(struct TetherActor *p)
{
    if (p->state != 0)
        TetherActor_SetState(p, 0);
    {
        u16 *q = &p->chargeCollider.flags;
        u32 v = *q;
        u32 m = 0xFFFB;
        u32 z;
        m &= v;
        z = 0;
        *q = m;
        p->glowMode = z;
    }
    TetherActor_SetEntry(p, 0);
    if (!(gUnk_030053F4 & 0x200)) {
        if (p->kind != 9) {
            u16 *r3 = &p->interactionLock;
            if (*r3 != 0) {
                TetherActor_UpdateLockedShake(p);
            } else {
                u32 v = p->chargeReady;
                if (v != 0) {
                    TetherActor_UpdateShake(p);
                } else if (p->blendRequested != 0) {
                    u32 t = p->secondaryMaximum;
                    u32 t2;
                    p->secondaryValue = t;
                    t2 = p->maximumValue;
                    p->currentValue = t2;
                    *r3 = v;
                } else {
                    TetherActor_UpdateInteractionValue(p);
                }
            }
        }
    }
}
