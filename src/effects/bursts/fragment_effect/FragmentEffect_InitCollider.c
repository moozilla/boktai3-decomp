#include "effects/bursts/fragment_effect.h"
#include "scene/collision/packed_contact_vector.h"

void Collider_SetAttackProperties(struct ContactCollider *, u32, u32, u32, u32, u32);
void FragmentEffect_OnColliderContact(void);

void FragmentEffect_InitCollider(struct Fragment *fragment, u32 a, u32 b, u32 c, u32 d)
{
    struct ContactCollider *collider;
    struct PackedContactVector size, offset;

    size.x = 0x18;
    size.y = 0x80;
    size.z = 0x18;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    collider = &fragment->collider;
    Collider_Init(collider, 0, 0x2401, 0, 0x10, &size, &offset);
    Collider_SetAttackProperties(collider, a, b, 0, c, d);
    Collider_SetCallback(collider, FragmentEffect_OnColliderContact, fragment);
    Collider_SetPosition(collider, &fragment->sprite.position, 0);
}
