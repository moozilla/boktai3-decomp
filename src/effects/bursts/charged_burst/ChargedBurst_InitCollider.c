#include "effects/bursts/charged_burst.h"
#include "scene/collision/packed_contact_vector.h"

void Collider_SetAttackProperties(struct ContactCollider *, s32, s32, s32, s32, s32);

void ChargedBurst_InitCollider(struct ChargedBurst *burst, u32 contactFlags, s32 power, s32 b)
{
    struct ContactCollider *collider = &burst->collider;
    struct PackedContactVector size;
    struct PackedContactVector offset;

    size.x = 100;
    size.y = 100;
    size.z = 100;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2100, 0, (u16)contactFlags, &size, &offset);
    Collider_SetAttackProperties(collider, power, 0, 0x20000, 0, 0);
    collider->direction = b;
    Collider_SetCallback(collider, 0, burst);
    Collider_SetPosition(collider, &burst->core.position, 0);
}
