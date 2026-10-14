#include "orbit_contact_actor.h"
void OrbitContactActor_UpdateInactive(struct OrbitContactActor *actor)
{
    if (actor->stateFlag) {
        actor->sprite.flags |= 1;
        actor->radius = 0;
        actor->angle = 192;
    }
    ++actor->stateFrame;
}
