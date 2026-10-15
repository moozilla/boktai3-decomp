#include "scene_steering_actor.h"
#define SceneSteeringActor_InitializeCollider sub_0806BBD0
void sub_0806BBB8(u32, u32, struct SceneSteeringActor *);
void SceneSteeringActor_InitializeCollider(struct SceneSteeringActor *p, u32 a, u32 b, u32 c, u32 d,
                                           u32 unusedE, u32 unusedF, u32 unusedG)
{
    union {
        struct ContactPackedVector packed;
        struct ContactPosition position;
    } size, offset;
    struct ContactCollider *collider;
    size.packed.x = 64;
    size.packed.y = 128;
    size.packed.z = 64;
    offset.packed.x = 0;
    offset.packed.y = 0;
    offset.packed.z = 0;
    collider = &p->collider;
    Collider_Init(collider, 0, 0x2403, 0, 16, &size.position, &offset.position);
    sub_0821FF58(collider, a, b, 0, c, d);
    Collider_SetCallback(collider, sub_0806BBB8, p);
    Collider_SetPosition(collider, &p->position, 0);
}
