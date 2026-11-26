#include "monsters/cannons/plasma_cannon.h"
void PlasmaCannon_TryFireAtPlayer(void);
u32 PlasmaCannon_UpdateRecoveryDelay(struct OrangeSpineActor *p)
{
    u16 n = p->recoveryTimer;
    if ((s16)p->recoveryTimer > 0) {
        p->recoveryTimer = n - 1;
    } else {
        Anim_SetSequence(&p->sequence, p->resource, 14, p->facing, 0);
        p->update = PlasmaCannon_TryFireAtPlayer;
    }
}
