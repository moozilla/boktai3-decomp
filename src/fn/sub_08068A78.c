#include "bouncing_object.h"
#define BouncingObject_InitializeColliders sub_08068A78
void BouncingObject_InitializeColliders(struct BouncingObject *object, s16 category, s16 value)
{
    union {
        struct ContactPackedVector packed;
        struct ContactPosition position;
    } size, offset;
    size.packed.x = 32;
    size.packed.y = 32;
    size.packed.z = 32;
    offset.packed.x = 0;
    offset.packed.y = 32;
    offset.packed.z = 0;
    Collider_Init(&object->bodyCollider, (u16)object->colliderConfig, 0x2003, 0, 16, &size.position,
                  &offset.position);
    sub_0821FF58(&object->bodyCollider, category, value, 0, 0, 0);
    Collider_SetPosition(&object->bodyCollider, &object->sprite.position, 0);
    Collider_SetCallback(&object->bodyCollider, sub_080688D0, object);
    size.position.x = 64;
    size.position.y = 128;
    size.position.z = 64;
    offset.position.x = 0;
    offset.position.y = 128;
    offset.position.z = 0;
    Collider_Init(&object->removalCollider, (u16)object->colliderConfig, 0x4001, 0, 16,
                  &size.position, &offset.position);
    Collider_SetPosition(&object->removalCollider, &object->sprite.position, 0);
    Collider_SetCallback(&object->removalCollider, sub_08068A3C, object);
    Collider_Refresh(&object->removalCollider);
}
