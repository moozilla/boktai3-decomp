#include "orbit_contact_actor.h"
void OrbitContactActor_BeginExpansion(struct OrbitContactActor *actor)
{
    u32 one = 1;
    u32 zero = 0;
    actor->state = one;
    actor->stateFlag = one;
    actor->stateFrame = zero;
    actor->angle = 192;
    actor->radius = zero;
}
