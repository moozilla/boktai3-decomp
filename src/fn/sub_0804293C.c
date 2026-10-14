#include "contact_actor.h"
void sub_080472A4(void *, u32, s32, void *, const struct ContactPackedVector *, u32);
void SceneActors_AddResourceCost(u32);
void ContactActor_ApplyResourceContact(void *source, struct ContactCollider *other,
                                       struct ContactActor *owner)
{
    struct ContactPackedVector size;
    size.x = 16;
    size.y = 0;
    size.z = 16;
    sub_080472A4(source, other->cost, 1, &owner->position, &size, 1);
    SceneActors_AddResourceCost(other->cost);
}
