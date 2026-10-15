#include "tether_actor.h"
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_SetEntry(struct TetherActor *, u32);
#define TetherActor_UpdatePassiveState sub_0804CDE8
void TetherActor_UpdatePassiveState(struct TetherActor *actor)
{
    if (actor->state != 12)
        TetherActor_SetState(actor, 12);
    actor->glowMode = 0;
    actor->interactionLock = 0;
    actor->chargeCollider.flags |= 4;
    actor->heading = 0;
    TetherActor_SetEntry(actor, 0);
}
