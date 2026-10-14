#include "tether_actor.h"
void TetherActor_ApplyPendingEffect(struct TetherActor *);
void TetherActor_UpdatePendingEffect(struct TetherActor *actor)
{
    TetherActor_ApplyPendingEffect(actor);
}
