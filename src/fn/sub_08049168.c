#include "tether_actor.h"
#define TetherActor_SetState sub_08049168
void TetherActor_SetState(struct TetherActor *actor, u32 state)
{
    actor->state = state;
    actor->stateFrame = 0;
}
