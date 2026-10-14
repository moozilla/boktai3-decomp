#include "orbit_contact_actor.h"
#define OrbitContactActor_Destroy sub_08046A00
s32 OrbitContactActor_Destroy(struct OrbitContactActor *actor)
{
    Sprite_Remove(&actor->sprite);
    return 0;
}
