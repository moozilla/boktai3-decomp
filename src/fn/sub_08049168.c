#include "tether_actor.h"
void TetherActor_SetState(struct TetherActor *actor, u32 state)
{
    actor->state = state;
    actor->stateFrame = 0;
}
