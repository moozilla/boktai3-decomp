#include "tether_actor.h"
#define TetherActor_UpdatePendingEffect sub_080492A4
#define TetherActor_ApplyPendingEffect  sub_08049258
void TetherActor_ApplyPendingEffect(struct TetherActor *);
void TetherActor_UpdatePendingEffect(struct TetherActor *actor)
{
    TetherActor_ApplyPendingEffect(actor);
}
