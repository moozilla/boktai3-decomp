#include "tether_actor.h"
void TetherActor_UpdatePalette(struct TetherActor *);
void TetherActor_UpdateMovingEffects(struct TetherActor *);
void TetherActor_UpdateSegments(struct TetherActor *);
void TetherActor_UpdateEffects(struct TetherActor *actor)
{
    TetherActor_UpdatePalette(actor);
    TetherActor_UpdateMovingEffects(actor);
    TetherActor_UpdateSegments(actor);
}
