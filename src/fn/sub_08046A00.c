#include "orbit_contact_actor.h"
s32 OrbitContactActor_Destroy(struct OrbitContactActor *actor)
{
    Sprite_Remove(&actor->sprite);
    return 0;
}
