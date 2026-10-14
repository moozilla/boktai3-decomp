#include "tether_actor.h"
#define TetherActor_UpdateEffects sub_0804914C
#define TetherActor_UpdatePalette sub_08048F40
#define TetherActor_UpdateMovingEffects sub_08049020
void TetherActor_UpdatePalette(struct TetherActor *);
void TetherActor_UpdateMovingEffects(struct TetherActor *);
void TetherActor_UpdateSegments(struct TetherActor *);
void TetherActor_UpdateEffects(struct TetherActor *actor)
{
    TetherActor_UpdatePalette(actor);
    TetherActor_UpdateMovingEffects(actor);
    TetherActor_UpdateSegments(actor);
}
