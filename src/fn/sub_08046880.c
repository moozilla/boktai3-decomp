#include "orbit_contact_actor.h"
#define OrbitContactActor_UpdateInactive sub_08046880
void OrbitContactActor_UpdateInactive(struct OrbitContactActor *actor)
{
    if (actor->stateFlag) {
        actor->sprite.flags |= 1;
        actor->radius = 0;
        actor->angle = 192;
    }
    ++actor->stateFrame;
}
