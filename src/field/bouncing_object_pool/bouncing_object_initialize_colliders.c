#include "field/bouncing_object_pool/bouncing_object.h"
void BouncingObject_InitializeColliders(struct BouncingObject *object, s16 category, s16 value)
{
    struct ContactPosition size, offset;
    size.x = 32;
    size.y = 32;
    size.z = 32;
    offset.x = 0;
    offset.y = 32;
    offset.z = 0;
    Collider_Init(&object->bodyCollider, (u16)object->colliderConfig, 0x2003, 0, 16, &size,
                  &offset);
    Collider_SetAttackProperties(&object->bodyCollider, category, value, 0, 0, 0);
    Collider_SetPosition(&object->bodyCollider, &object->sprite.position, 0);
    Collider_SetCallback(&object->bodyCollider, BouncingObject_ApplyContactImpulse, object);
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(&object->removalCollider, (u16)object->colliderConfig, 0x4001, 0, 16,
                  &size, &offset);
    Collider_SetPosition(&object->removalCollider, &object->sprite.position, 0);
    Collider_SetCallback(&object->removalCollider, BouncingObject_HandleTimerContact, object);
    Collider_Refresh(&object->removalCollider);
}
