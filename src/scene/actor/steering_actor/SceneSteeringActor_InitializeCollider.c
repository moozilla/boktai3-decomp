#include "scene/motion/scene_steering_actor.h"
void SceneSteeringActor_RequestDeparture(u32, u32, struct SceneSteeringActor *);
void SceneSteeringActor_InitializeCollider(struct SceneSteeringActor *p, u32 a, u32 b, u32 c, u32 d,
                                           u32 unusedE, u32 unusedF, u32 unusedG)
{
    struct ContactPosition size, offset;
    struct ContactCollider *collider;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    collider = &p->collider;
    Collider_Init(collider, 0, 0x2403, 0, 16, &size, &offset);
    Collider_SetAttackProperties(collider, a, b, 0, c, d);
    Collider_SetCallback(collider, SceneSteeringActor_RequestDeparture, p);
    Collider_SetPosition(collider, &p->position, 0);
}
