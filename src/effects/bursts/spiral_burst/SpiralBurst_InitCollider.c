#include "effects/bursts/spiral_burst.h"
#include "scene/collision/packed_contact_vector.h"

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void SpiralBurst_InitCollider(struct SpiralBurst *burst)
{
    struct PackedContactVector size, offset;
    u8 *collider = burst->collider;

    size.x = 100;
    size.y = 50;
    size.z = 100;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2000, 0, 1, &size, &offset);
    Collider_SetAttackProperties(collider, 0, 0, 0x40000, 0, 0);
    Collider_SetCallback(collider, 0, burst);
    Collider_SetPosition(collider, &burst->origin, 0);
}
