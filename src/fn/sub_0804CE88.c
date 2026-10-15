#include "tether_actor.h"
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_UpdateSpecialEntryState(struct TetherActor *actor)
{
    if (actor->state != 13)
        TetherActor_SetState(actor, 13);
    if (actor->kind == 6)
        actor->glowMode = 1;
    else if (actor->kind == 7) {
        actor->blendHold = 1;
        actor->glowMode = 0;
    } else
        actor->glowMode = 0;
    actor->chargeCollider.flags |= 4;
    actor->heading = 0;
    TetherActor_SetEntry(actor, 27);
}
