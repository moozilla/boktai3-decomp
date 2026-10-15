#include "scene_steering_actor.h"
void SceneSteeringActor_RefreshCollider(struct SceneSteeringActor *p)
{
    struct ContactCollider *collider = &p->collider;
    Collider_SetPosition(collider, &p->position, 0);
    Collider_Refresh(collider);
}
